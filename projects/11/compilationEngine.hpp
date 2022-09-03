#ifndef COMPIL_ENGINE_H
#define COMPIL_ENGINE_H

#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>
#include "globalDefine.h"
#include "jackTokenizer.hpp"
#include "symbolTabel.hpp"
#include "vmWriter.hpp"
using namespace std;

class CompilationEngine {
   public:
    CompilationEngine(Tokenizer& tokenizer, VMWriter& vmWriter);
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
    VMWriter& vmWriter;
    Tokenizer& tokenizer;
    SymbolTabel symbolTabel;
    string nowClassName, nowFunctionName;
    Keyword nowSubroutineType;
    void readKeyword(Keyword type, string name);
    void readSymbol(char symbol);
    void readIdentifier();
    void readStringConst();
    void readIntConst();
    void readType();
    void readOp();
    void readUnaryOp();
    // void writeToFile(string tag, string text = "");
    void getNextToken();
    void compilationError(string what);

    bool checkStatementKeyword(string key);
    bool checkOperator(char c);
};
CompilationEngine::CompilationEngine(Tokenizer& tokenizer, VMWriter& vmWriter)
    : tokenizer(tokenizer), vmWriter(vmWriter) {}
void CompilationEngine::compileClass() {
    // 'class' className '{' classVarDec* subroutineDec* '}'
    getNextToken();
    readKeyword(Keyword::kCLASS, "class");
    getNextToken();
    readIdentifier();
    nowClassName = tokenizer.identifier();  // 获取当前类的名称
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

    if (tokenizer.advance())  // 一个文件中只能有一个类
        compilationError(
            "jack language does not allow anything else beyond the class!");
}
void CompilationEngine::compileClassVarDec() {
    // ('static' | 'field') type varName (',' varName)* ';'

    if (tokenizer.keyword() == Keyword::kFIELD)
        readKeyword(Keyword::kFIELD, "field");
    else
        readKeyword(Keyword::kSTATIC, "static");

    string type = tokenizer.identifier();
    Kind kind = keyWord2Kind(tokenizer.keyword());

    getNextToken();
    readType();
    getNextToken();
    readIdentifier();
    symbolTabel.define(tokenizer.identifier(), type, kind);
    getNextToken();

    while (tokenizer.tokenType() == TokenType::kSYMBOL and
           tokenizer.symbol() == ',') {
        readSymbol(',');
        getNextToken();
        readIdentifier();
        symbolTabel.define(tokenizer.identifier(), type, kind);
        getNextToken();
    }

    readSymbol(';');
    getNextToken();
}
void CompilationEngine::compileSubroutine() {
    // ('constructor' | 'function' | 'method') ('void' | type) subroutineName
    // '(' parameterList ')' subroutineBody
    symbolTabel.startSubroutine();

    if (tokenizer.keyword() == Keyword::kCONSTRUCTOR)
        readKeyword(Keyword::kCONSTRUCTOR, "constructor");
    else if (tokenizer.keyword() == Keyword::kFUNCTION)
        readKeyword(Keyword::kFUNCTION, "function");
    else {
        symbolTabel.define("this", nowClassName, Kind::ARG);
        readKeyword(Keyword::kMETHOD, "method");
    }
    nowSubroutineType = tokenizer.keyword();  // 得到当前函数类型

    getNextToken();

    string returnType;
    if (tokenizer.tokenType() == TokenType::kKEYWORD and
        tokenizer.keyword() == Keyword::kVOID) {
        readKeyword(Keyword::kVOID, "void");
        returnType = "void";
    } else {
        readType();
        returnType = tokenizer.identifier();
    }

    getNextToken();
    readIdentifier();  // 获得函数名
    nowFunctionName = nowClassName + string(".") + tokenizer.identifier();
    getNextToken();
    readSymbol('(');
    getNextToken();
    compileParameterList();

    readSymbol(')');
    getNextToken();
    compileSubroutineBody();
}
void CompilationEngine::compileParameterList() {
    // ((type varName)(',' type varName)*)?
    Kind kind = Kind::ARG;

    if (tokenizer.tokenType() != TokenType::kSYMBOL) {
        readType();
        string type = tokenizer.identifier();
        getNextToken();
        readIdentifier();
        symbolTabel.define(tokenizer.identifier(), type, kind);
        getNextToken();

        while (tokenizer.tokenType() == TokenType::kSYMBOL and
               tokenizer.symbol() == ',') {
            readSymbol(',');
            getNextToken();
            readType();
            type = tokenizer.identifier();
            getNextToken();
            readIdentifier();
            symbolTabel.define(tokenizer.identifier(), type, kind);
            getNextToken();
        }
    }
}
void CompilationEngine::compileVarDec() {
    // 'var' type varName (',' varName)* ';'

    readKeyword(Keyword::kVAR, "var");
    Kind kind = Kind::VAR;
    getNextToken();
    readType();
    string type = tokenizer.identifier();
    getNextToken();
    readIdentifier();
    symbolTabel.define(tokenizer.identifier(), type, kind);
    getNextToken();
    while (tokenizer.tokenType() == TokenType::kSYMBOL and
           tokenizer.symbol() == ',') {
        readSymbol(',');
        getNextToken();
        readIdentifier();
        symbolTabel.define(tokenizer.identifier(), type, kind);
        getNextToken();
    }
    readSymbol(';');

    getNextToken();
}
void CompilationEngine::compileStatements() {
    // statements: statement*
    //    statement: letStatement | ifStatement | whileStatement | doStatement |
    //    returnStatement

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
}
void CompilationEngine::compileDo() {
    // 'do' subroutineCall ';'

    readKeyword(Keyword::kDO, "do");
    getNextToken();
    compileSubroutineCall();
    readSymbol(';');

    getNextToken();
}
void CompilationEngine::compileLet() {
    // 'let' varName('[' expression ']')? '=' expression ';'

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

    getNextToken();
}
void CompilationEngine::compileWhile() {
    // 'while' '(' expression ')' '{' statements '}'

    readKeyword(Keyword::kWHILE, "while");
    getNextToken();
    readSymbol('(');
    getNextToken();
    compileExpression();
    readSymbol(')');
    getNextToken();
    readSymbol('{');
    getNextToken();
    compileStatements();
    readSymbol('}');

    getNextToken();
}
void CompilationEngine::compileReturn() {
    // 'return' expression? ';'

    readKeyword(Keyword::kRETURN, "return");
    getNextToken();

    if (tokenizer.tokenType() != TokenType::kSYMBOL ||
        (tokenizer.tokenType() == TokenType::kSYMBOL &&
         tokenizer.symbol() != ';'))
        compileExpression();

    readSymbol(';');

    getNextToken();
}
void CompilationEngine::compileIf() {
    // 'if' '(' expression ')' '{' statements '}'
    //  ('else' '{' statements '}')?

    readKeyword(Keyword::kIF, "if");
    getNextToken();
    readSymbol('(');
    getNextToken();
    compileExpression();
    readSymbol(')');
    getNextToken();
    readSymbol('{');
    getNextToken();
    compileStatements();
    readSymbol('}');
    getNextToken();

    if (tokenizer.tokenType() == TokenType::kKEYWORD and
        tokenizer.keyword() == Keyword::kELSE) {
        readKeyword(Keyword::kELSE, "else");
        getNextToken();
        readSymbol('{');
        getNextToken();
        compileStatements();
        readSymbol('}');
        getNextToken();
    }
}
void CompilationEngine::compileExpression() {
    // term (op term)*
    compileTerm();
    while (tokenizer.tokenType() == TokenType::kSYMBOL and
           checkOperator(tokenizer.symbol())) {
        readOp();
        getNextToken();
        compileTerm();
    }
}
void CompilationEngine::compileTerm() {
    //  integerConst | stringConst | keywordConst |
    //    varName | varName '[' expression ']' | subroutineCall |
    //    '(' expression ')' | unaryOp term

    switch (tokenizer.tokenType()) {
        case TokenType::kINT_CONST:
            readIntConst();
            break;
        case TokenType::kSTRING_CONST:
            readStringConst();
            break;
        case TokenType::kKEYWORD: {
            switch (tokenizer.keyword()) {
                case Keyword::kTRUE:
                    readKeyword(Keyword::kTRUE, "true");
                    break;
                case Keyword::kFALSE:
                    readKeyword(Keyword::kFALSE, "false");
                    break;
                case Keyword::kNULL:
                    readKeyword(Keyword::kNULL, "null");
                    break;
                case Keyword::kTHIS:
                    readKeyword(Keyword::kTHIS, "this");
                    break;
            }
            break;
        }
        case TokenType::kIDENTIFIER: {
            readIdentifier();
            getNextToken();
            if (tokenizer.tokenType() == TokenType::kSYMBOL and
                tokenizer.symbol() == '[') {
                readSymbol('[');
                getNextToken();
                compileExpression();
                readSymbol(']');
                getNextToken();
            } else if (tokenizer.tokenType() == TokenType::kSYMBOL and
                       tokenizer.symbol() == '(') {
                readSymbol('(');
                getNextToken();
                compileExpressionList();
                readSymbol(')');
                getNextToken();
            } else if (tokenizer.tokenType() == TokenType::kSYMBOL and
                       tokenizer.symbol() == '.') {
                readSymbol('.');
                getNextToken();
                readIdentifier();
                getNextToken();
                readSymbol('(');
                getNextToken();
                compileExpressionList();
                readSymbol(')');
                getNextToken();
            }
            return;
        }
        case TokenType::kSYMBOL:
            if (tokenizer.symbol() == '(') {
                readSymbol('(');
                getNextToken();
                compileExpression();
                readSymbol(')');
            } else {
                readUnaryOp();
                getNextToken();
                compileTerm();

                return;
            }
            break;
        default:
            break;
    }

    getNextToken();
}
void CompilationEngine::compileExpressionList() {
    // (expression(',' expression)*)?

    if (tokenizer.tokenType() != TokenType::kSYMBOL ||
        (tokenizer.symbol() == '('))
        compileExpression();
    while (tokenizer.tokenType() == TokenType::kSYMBOL &&
           tokenizer.symbol() == ',') {
        readSymbol(',');
        getNextToken();
        compileExpression();
    }
}

void CompilationEngine::compileSubroutineBody() {
    // '{' varDec* statements '}'
    readSymbol('{');
    getNextToken();

    while (tokenizer.tokenType() == TokenType::kKEYWORD and
           tokenizer.keyword() == Keyword::kVAR)
        compileVarDec();

    int localVarNums = symbolTabel.varCount(Kind::VAR);
    vmWriter.writeFunction(nowFunctionName, localVarNums);
    if (nowSubroutineType == Keyword::kCONSTRUCTOR) {
        vmWriter.writePush(Segment::CONST, symbolTabel.varCount(Kind::FIELD));
        vmWriter.writeCall("Memory.alloc", 1);   // 申请内存
        vmWriter.writePop(Segment::POINTER, 0);  // 保存到this
    } else if (nowSubroutineType == Keyword::kMETHOD) {
        vmWriter.writePush(Segment::ARG, 0);  // 将this指针导入
        vmWriter.writePop(Segment::POINTER, 0);
    }

    compileStatements();
    readSymbol('}');
    getNextToken();
}
void CompilationEngine::compileSubroutineCall() {
    //  subroutineName '(' expressionList ')' |
    //   (className | varName) '.' subroutineName '(' expression ')'

    readIdentifier();
    getNextToken();

    if (tokenizer.tokenType() == TokenType::kSYMBOL &&
        tokenizer.symbol() == '(') {
        readSymbol('(');
        getNextToken();
        compileExpressionList();
        readSymbol(')');
    } else {
        readSymbol('.');
        getNextToken();
        readIdentifier();
        getNextToken();
        readSymbol('(');
        getNextToken();
        compileExpressionList();
        readSymbol(')');
    }

    getNextToken();
}

void CompilationEngine::readKeyword(Keyword type, string name) {
    if (tokenizer.tokenType() != TokenType::kKEYWORD or
        tokenizer.keyword() != type)
        compilationError("unkown keyword");
}
void CompilationEngine::readSymbol(char symbol) {
    if (tokenizer.tokenType() != TokenType::kSYMBOL or
        tokenizer.symbol() != symbol)
        compilationError("unkown symbol");
}
void CompilationEngine::readIdentifier() {
    if (tokenizer.tokenType() != TokenType::kIDENTIFIER)
        compilationError("unkown identifier");
}
void CompilationEngine::readStringConst() {
    if (tokenizer.tokenType() != TokenType::kSTRING_CONST)
        compilationError("stringConstant");
}
void CompilationEngine::readIntConst() {
    if (tokenizer.tokenType() != TokenType::kINT_CONST)
        compilationError("integerConstant");
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
void CompilationEngine::readOp() {
    if (tokenizer.tokenType() == TokenType::kSYMBOL) {
        char sym = tokenizer.symbol();
        if (checkOperator(sym)) {
            // writeToFile("symbol", tokenizer.token);
            return;
        }
    }
    compilationError("op");
}
void CompilationEngine::readUnaryOp() {
    if (tokenizer.tokenType() == TokenType::kSYMBOL) {
        char sym = tokenizer.symbol();
        if (sym == '-' || sym == '~') {
            // writeToFile("symbol", tokenizer.token);
            return;
        }
    }
    compilationError("unary op");
}

// void CompilationEngine::writeToFile(string tag, string text) {
//     if (text.empty())
//         outputStream << "<" << tag << ">" << endl;
//     else
//         outputStream << "<" << tag << ">"
//                      << " " << text << " "
//                      << "</" << tag << ">" << endl;
// }
void CompilationEngine::getNextToken() {
    if (!tokenizer.advance())
        compilationError("Could not read next token!");
}
void CompilationEngine::compilationError(string what) {
    cerr << "ERROR: in line " << tokenizer.linePos << " :" << what << endl;
    exit(1);
}

bool CompilationEngine::checkStatementKeyword(string key) {
    return (key == "let" || key == "if" || key == "while" || key == "do" ||
            key == "return");
}
bool CompilationEngine::checkOperator(char ch) {
    return (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '&' ||
            ch == '|' || ch == '<' || ch == '>' || ch == '=');
}
#endif
