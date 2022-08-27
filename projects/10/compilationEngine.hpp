#ifndef COMPIL_ENGINE_H
#define COMPIL_ENGINE_H

#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>
#include "globalDefine.h"
#include "jackTokenizer.hpp"
using namespace std;

class CompilationEngine {
   public:
    CompilationEngine(Tokenizer& tokenizer, ostream& outputStream);
    void compile();
    void compileClass();
    void compileClassVarDec();
    void compileSubroutine();
    void compileParameterList();
    void compileVarDec();
    void compileStatements();
    void compileDo();
    void compileLet();
    void compileWhile();
    void compileReturn();
    void compileIf();
    void compileExpression();
    void compileTerm();
    void compileExpressionList();

    void compileSubroutineBody();
    void compileSubroutineCall();

   private:
    ostream& outputStream;
    Tokenizer& tokenizer;
    void readKeyword(Keyword type, string name);
    void readSymbol(char symbol);
    void readIdentifier();
    void readStringConst();
    void readIntConst();
    void readType();
    void writeToFile(string tag, string text = "");
    void getNextToken();
    void compilationError(string what);

    bool checkStatementKeyword(string key);
};
CompilationEngine::CompilationEngine(Tokenizer& tokenizer,
                                     ostream& outputStream)
    : tokenizer(tokenizer), outputStream(outputStream) {}
void CompilationEngine::compileClass() {
    // 'class' className '{' classVarDec* subroutineDec* '}'
    writeToFile("class");
    getNextToken();
    readKeyword(Keyword::kCLASS, "class");
    getNextToken();
    readIdentifier();
    getNextToken();
    readSymbol('{');
    getNextToken();

    while (tokenizer.tokenType() == TokenType::kKEYWORD &&
           (tokenizer.keyword() == Keyword::kFIELD ||
            tokenizer.keyword() == Keyword::kSTATIC))
        compileClassVarDec();

    while (tokenizer.tokenType() == TokenType::kKEYWORD &&
           (tokenizer.keyword() == Keyword::kCONSTRUCTOR ||
            tokenizer.keyword() == Keyword::kFUNCTION ||
            tokenizer.keyword() == Keyword::kMETHOD))
        compileSubroutine();

    readSymbol('}');
    writeToFile("/class");

    if (tokenizer.advance())  // 一个文件中只能有一个类
        compilationError(
            "jack language does not allow anything else beyond the class!");
}
void CompilationEngine::compileClassVarDec() {
    // ('static' | 'field') type varName (',' varName)* ';'
    writeToFile("classVarDec");

    if (tokenizer.keyword() == Keyword::kFIELD)
        readKeyword(Keyword::kFIELD, "field");
    else
        readKeyword(Keyword::kSTATIC, "static");

    getNextToken();
    readType();
    getNextToken();
    readIdentifier();
    getNextToken();

    while (tokenizer.tokenType() == TokenType::kSYMBOL and
           tokenizer.symbol() == ',') {
        readSymbol(',');
        getNextToken();
        readIdentifier();
        getNextToken();
    }

    readSymbol(';');
    writeToFile("/classVarDec");
    getNextToken();
}
void CompilationEngine::compileSubroutine() {
    // ('constructor' | 'function' | 'method') ('void' | type) subroutineName
    // '(' parameterList ')' subroutineBody
    writeToFile("subroutineDec");

    if (tokenizer.keyword() == Keyword::kCONSTRUCTOR)
        readKeyword(Keyword::kCONSTRUCTOR, "constructor");
    else if (tokenizer.keyword() == Keyword::kFUNCTION)
        readKeyword(Keyword::kFUNCTION, "function");
    else
        readKeyword(Keyword::kMETHOD, "method");

    getNextToken();

    if (tokenizer.tokenType() == TokenType::kKEYWORD and
        tokenizer.keyword() == Keyword::kVOID)
        readKeyword(Keyword::kVOID, "void");
    else
        readType();

    getNextToken();
    readIdentifier();
    getNextToken();
    readSymbol('(');
    getNextToken();
    compileParameterList();  // tag

    readSymbol(')');
    getNextToken();
    compileSubroutineBody();

    writeToFile("/subroutineDec");
}
void CompilationEngine::compileParameterList() {
    // ((type varName)(',' type varName)*)?
    writeToFile("parameterList");
    if (tokenizer.tokenType() != TokenType::kSYMBOL) {
        readType();
        getNextToken();
        readIdentifier();
        getNextToken();

        while (tokenizer.tokenType() == TokenType::kSYMBOL and
               tokenizer.symbol() == ',') {
            readSymbol(',');
            getNextToken();
            readType();
            getNextToken();
            readIdentifier();
            getNextToken();
        }
    }
    writeToFile("/parameterList");
}
void CompilationEngine::compileVarDec() {
    // 'var' type varName (',' varName)* ';'
    writeToFile("varDec");

    readKeyword(Keyword::kVAR, "var");
    getNextToken();
    readType();
    getNextToken();
    readIdentifier();
    getNextToken();
    while (tokenizer.tokenType() == TokenType::kSYMBOL and
           tokenizer.symbol() == ',') {
        readSymbol(',');
        getNextToken();
        readIdentifier();
        getNextToken();
    }
    readSymbol(';');

    writeToFile("/varDec");
    getNextToken();
}
void CompilationEngine::compileStatements() {
    // statements: statement*
    //    statement: letStatement | ifStatement | whileStatement | doStatement |
    //    returnStatement
    writeToFile("statements");

    while (tokenizer.tokenType() == TokenType::kKEYWORD and
           checkStatementKeyword(tokenizer.token)) {
        switch (tokenizer.keyword()) {
            case Keyword::kLET:
                compileLet();
                break;
            case Keyword::kDO:
                compileDo();
                break;
            case Keyword::kIF:
                compileIf();
                break;
            case Keyword::kWHILE:
                compileWhile();
                break;
            case Keyword::kRETURN:
                compileReturn();
                break;
            default:
                break;
        }
    }

    writeToFile("/statements");
}
void CompilationEngine::compileDo() {
    // 'do' subroutineCall ';'
    writeToFile("doStatement");

    readKeyword(Keyword::kDO, "do");
    getNextToken();
    compileSubroutineCall();
    readSymbol(';');

    writeToFile("/doStatement");
    getNextToken();
}
void CompilationEngine::compileLet() {
    // 'let' varName('[' expression ']')? '=' expression ';'
    writeToFile("letStatement");

    readKeyword(Keyword::kLET, "let");
    getNextToken();
    readIdentifier();
    getNextToken();

    if (tokenizer.tokenType() == TokenType::kSYMBOL and
        tokenizer.symbol() == '[') {
        readSymbol('[');
        getNextToken();
        compileExpression();
        readSymbol(']');
        getNextToken();
    }

    readSymbol('=');
    getNextToken();
    compileExpression();
    readSymbol(';');

    writeToFile("/letStatement");
    getNextToken();
}
void CompilationEngine::compileWhile(){};
void CompilationEngine::compileReturn(){};
void CompilationEngine::compileIf(){};
void CompilationEngine::compileExpression(){};
void CompilationEngine::compileTerm(){};
void CompilationEngine::compileExpressionList(){};

void CompilationEngine::compileSubroutineBody() {
    // '{' varDec* statements '}'
    writeToFile("subroutineBody");
    readSymbol('{');
    getNextToken();

    while (tokenizer.tokenType() == TokenType::kKEYWORD and
           tokenizer.keyword() == Keyword::kVAR)
        compileVarDec();

    compileStatements();
    readSymbol('}');
    writeToFile("/subroutineBody");
    getNextToken();
}
void CompilationEngine::compileSubroutineCall(){};

void CompilationEngine::readKeyword(Keyword type, string name) {
    if (tokenizer.tokenType() == TokenType::kKEYWORD and
        tokenizer.keyword() == type)
        writeToFile("keyword", tokenizer.token);
    else
        compilationError("");
}
void CompilationEngine::readSymbol(char symbol) {
    if (tokenizer.tokenType() == TokenType::kSYMBOL and
        tokenizer.symbol() == symbol)
        writeToFile("symbol", tokenizer.token);
    else
        compilationError("");
}
void CompilationEngine::readIdentifier() {
    if (tokenizer.tokenType() == TokenType::kIDENTIFIER)
        writeToFile("identifier", tokenizer.token);
    else
        compilationError("");
}
void CompilationEngine::readStringConst() {
    if (tokenizer.tokenType() == TokenType::kSTRING_CONST)
        writeToFile("stringConstant", tokenizer.token);
    else
        compilationError("");
}
void CompilationEngine::readIntConst() {
    if (tokenizer.tokenType() == TokenType::kINT_CONST)
        writeToFile("integerConstant", tokenizer.token);
    else
        compilationError("");
}
void CompilationEngine::readType() {
    // int|char|boolean|className
    if (tokenizer.tokenType() == TokenType::kKEYWORD) {
        switch (tokenizer.keyword()) {
            case Keyword::kINT:
                readKeyword(Keyword::kINT, "int");
                break;

            case Keyword::kCHAR:
                readKeyword(Keyword::kCHAR, "char");
                break;

            case Keyword::kBOOLEAN:
                readKeyword(Keyword::kBOOLEAN, "boolean");
                break;
            default:
                break;
        }
    } else
        readIdentifier();
}

void CompilationEngine::writeToFile(string tag, string text) {
    if (text.empty())
        outputStream << "<" << tag << ">" << endl;
    else
        outputStream << "<" << tag << ">"
                     << " " << text << " "
                     << "</" << tag << ">" << endl;
}
void CompilationEngine::getNextToken() {
    if (!tokenizer.advance())
        compilationError("Could not read next token!");
}
void CompilationEngine::compilationError(string what) {
    cerr << "Compilation ERROR: " << what << endl;
    exit(1);
}

bool CompilationEngine::checkStatementKeyword(string key) {
    return (key == "let" || key == "if" || key == "while" || key == "do" ||
            key == "return");
}
#endif
