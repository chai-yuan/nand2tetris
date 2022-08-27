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
};
CompilationEngine::CompilationEngine(Tokenizer& tokenizer,
                                     ostream& outputStream)
    : tokenizer(tokenizer), outputStream(outputStream) {}
void CompilationEngine::compileClass() {
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
void CompilationEngine::compileClassVarDec(){};
void CompilationEngine::compileSubroutine(){};
void CompilationEngine::compileParameterList(){};
void CompilationEngine::compileVarDec(){};
void CompilationEngine::compileStatements(){};
void CompilationEngine::compileDo(){};
void CompilationEngine::compileLet(){};
void CompilationEngine::compileWhile(){};
void CompilationEngine::compileReturn(){};
void CompilationEngine::compileIf(){};
void CompilationEngine::compileExpression(){};
void CompilationEngine::compileTerm(){};
void CompilationEngine::compileExpressionList(){};

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
#endif
