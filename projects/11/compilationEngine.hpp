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
    int argNum;
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

    vmWriter.writePop(Segment::TEMP, 0);  // 忽略返回值
    getNextToken();
}
void CompilationEngine::compileLet() {
    // 'let' varName('[' expression ']')? '=' expression ';'

    readKeyword(Keyword::kLET, "let");
    getNextToken();
    readIdentifier();
    string varName = tokenizer.identifier();
    getNextToken();

    bool isArray = false;
    if (tokenizer.tokenType() == TokenType::kSYMBOL and
        tokenizer.symbol() == '[') {
        readSymbol('[');
        getNextToken();
        compileExpression();
        vmWriter.writePush(kind2Segment(symbolTabel.kindOf(varName)),
                           symbolTabel.indexOf(varName));
        vmWriter.writeArithmetic(Command::ADD);  // 通过add获得准确内存地址
        readSymbol(']');
        getNextToken();
        isArray = true;
    }

    readSymbol('=');
    getNextToken();
    compileExpression();
    readSymbol(';');

    if (!isArray)
        vmWriter.writePop(kind2Segment(symbolTabel.kindOf(varName)),
                          symbolTabel.indexOf(varName));
    else {
        vmWriter.writePop(Segment::TEMP, 0);  // 如果是数组，将结果临时保存
        vmWriter.writePop(Segment::POINTER, 1);  // ----------------
        vmWriter.writePush(Segment::TEMP, 0);
        vmWriter.writePop(Segment::THAT, 0);
    }

    getNextToken();
}
void CompilationEngine::compileWhile() {
    // 'while' '(' expression ')' '{' statements '}'
    string trueLabel = "WHILETRUE" + to_string(rand()),
           falseLabel = "WHILEFALSE" + to_string(rand());

    readKeyword(Keyword::kWHILE, "while");
    getNextToken();
    readSymbol('(');
    getNextToken();
    vmWriter.writeLabel(trueLabel);
    compileExpression();
    vmWriter.writeArithmetic(Command::NOT);
    vmWriter.writeIf(falseLabel);
    readSymbol(')');
    getNextToken();
    readSymbol('{');
    getNextToken();
    compileStatements();
    readSymbol('}');
    vmWriter.writeGoto(trueLabel);
    vmWriter.writeLabel(falseLabel);

    getNextToken();
}
void CompilationEngine::compileReturn() {
    // 'return' expression? ';'

    readKeyword(Keyword::kRETURN, "return");
    getNextToken();

    bool haveExpression = false;
    if (tokenizer.tokenType() != TokenType::kSYMBOL ||
        (tokenizer.tokenType() == TokenType::kSYMBOL &&
         tokenizer.symbol() != ';'))
        compileExpression(), haveExpression = true;  // -----------

    readSymbol(';');
    if (!haveExpression)
        vmWriter.writePush(Segment::CONST, 0);
    vmWriter.writeReturn();

    getNextToken();
}
void CompilationEngine::compileIf() {
    // 'if' '(' expression ')' '{' statements '}'
    //  ('else' '{' statements '}')?
    string elseLabel = "IFFALSE" + to_string(rand()),
           continueLabel = "CONTINUE" + to_string(rand());
    readKeyword(Keyword::kIF, "if");
    getNextToken();
    readSymbol('(');
    getNextToken();
    compileExpression();
    readSymbol(')');
    vmWriter.writeArithmetic(Command::NOT);
    vmWriter.writeIf(elseLabel);
    getNextToken();
    readSymbol('{');
    getNextToken();
    compileStatements();
    readSymbol('}');
    vmWriter.writeGoto(continueLabel);
    getNextToken();

    vmWriter.writeLabel(elseLabel);
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
    vmWriter.writeLabel(continueLabel);
}
void CompilationEngine::compileExpression() {
    // term (op term)*
    compileTerm();
    while (tokenizer.tokenType() == TokenType::kSYMBOL and
           checkOperator(tokenizer.symbol())) {
        readOp();
        char op = tokenizer.symbol();
        getNextToken();
        compileTerm();

        switch (op) {
            case '+':
                vmWriter.writeArithmetic(Command::ADD);
                break;
            case '-':
                vmWriter.writeArithmetic(Command::SUB);
                break;
            case '*':
                vmWriter.writeCall("Math.multiply", 2);
                break;
            case '/':
                vmWriter.writeCall("Math.divide", 2);
                break;
            case '&':
                vmWriter.writeArithmetic(Command::AND);
                break;
            case '|':
                vmWriter.writeArithmetic(Command::OR);
                break;
            case '<':
                vmWriter.writeArithmetic(Command::LT);
                break;
            case '>':
                vmWriter.writeArithmetic(Command::GT);
                break;
            case '=':
                vmWriter.writeArithmetic(Command::EQ);
                break;
            case '~':
                vmWriter.writeArithmetic(Command::NOT);
                break;
            default:
                break;
        }
        // compileTerm();
    }
}
void CompilationEngine::compileTerm() {
    //  integerConst | stringConst | keywordConst |
    //    varName | varName '[' expression ']' | subroutineCall |
    //    '(' expression ')' | unaryOp term
    string constString;
    switch (tokenizer.tokenType()) {
        case TokenType::kINT_CONST:
            readIntConst();
            vmWriter.writePush(Segment::CONST, tokenizer.intVal());
            break;
        case TokenType::kSTRING_CONST:
            readStringConst();
            constString = tokenizer.stringVal();
            vmWriter.writePush(Segment::CONST, constString.length());
            vmWriter.writeCall("String.new", 1);
            for (auto c : constString) {
                vmWriter.writePush(Segment::CONST, (int)c);
                vmWriter.writeCall("String.appendChar", 2);
            }
            break;
        case TokenType::kKEYWORD: {
            switch (tokenizer.keyword()) {
                case Keyword::kTRUE:
                    readKeyword(Keyword::kTRUE, "true");
                    vmWriter.writePush(Segment::CONST, 0);  // ------
                    vmWriter.writeArithmetic(Command::NOT);
                    break;
                case Keyword::kFALSE:
                    readKeyword(Keyword::kFALSE, "false");
                    vmWriter.writePush(Segment::CONST, 0);
                    break;
                case Keyword::kNULL:
                    readKeyword(Keyword::kNULL, "null");
                    vmWriter.writePush(Segment::CONST, 0);
                    break;
                case Keyword::kTHIS:
                    readKeyword(Keyword::kTHIS, "this");
                    vmWriter.writePush(Segment::POINTER, 0);
                    break;
            }
            break;
        }
        case TokenType::kIDENTIFIER: {
            readIdentifier();
            string id = tokenizer.identifier();
            getNextToken();
            if (tokenizer.tokenType() == TokenType::kSYMBOL and
                tokenizer.symbol() == '[') {
                readSymbol('[');
                getNextToken();
                compileExpression();
                vmWriter.writePush(kind2Segment(symbolTabel.kindOf(id)),
                                   symbolTabel.indexOf(id));
                vmWriter.writeArithmetic(Command::ADD);
                readSymbol(']');
                getNextToken();
                vmWriter.writePop(Segment::POINTER, 1);
                vmWriter.writePush(Segment::THAT, 0);
            } else if (tokenizer.tokenType() == TokenType::kSYMBOL and
                       tokenizer.symbol() == '(') {
                readSymbol('(');
                getNextToken();
                vmWriter.writePush(Segment::POINTER, 0);
                compileExpressionList();
                readSymbol(')');
                vmWriter.writeCall(nowClassName + "." + id, argNum + 1);
                getNextToken();
            } else if (tokenizer.tokenType() == TokenType::kSYMBOL and
                       tokenizer.symbol() == '.') {
                // 如果不是内置的
                bool buildIn = true;
                if (symbolTabel.kindOf(id) != Kind::NONE)
                    vmWriter.writePush(kind2Segment(symbolTabel.kindOf(id)),
                                       symbolTabel.indexOf(id)),
                        buildIn = false;

                readSymbol('.');
                getNextToken();
                readIdentifier();
                string subroutineName = tokenizer.identifier();
                getNextToken();
                readSymbol('(');
                getNextToken();
                compileExpressionList();
                readSymbol(')');

                if (!buildIn)
                    vmWriter.writeCall(
                        symbolTabel.typeOf(id) + "." + subroutineName,
                        argNum + 1);
                else
                    vmWriter.writeCall(id + "." + subroutineName, argNum);

                getNextToken();
            } else
                vmWriter.writePush(kind2Segment(symbolTabel.kindOf(id)),
                                   symbolTabel.indexOf(id));
            return;
        }
        case TokenType::kSYMBOL:
            if (tokenizer.symbol() == '(') {
                readSymbol('(');
                getNextToken();
                compileExpression();
                readSymbol(')');
            } else {
                char uop = tokenizer.symbol();
                readUnaryOp();
                getNextToken();
                compileTerm();
                if (uop == '-') {
                    vmWriter.writeArithmetic(Command::NEG);
                } else
                    vmWriter.writeArithmetic(Command::NOT);

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
    argNum = 0;
    if (tokenizer.tokenType() != TokenType::kSYMBOL ||
        (tokenizer.symbol() == '('))
        compileExpression(), argNum++;
    while (tokenizer.tokenType() == TokenType::kSYMBOL &&
           tokenizer.symbol() == ',') {
        readSymbol(',');
        argNum++;
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
    string callName = tokenizer.identifier();
    getNextToken();

    if (tokenizer.tokenType() == TokenType::kSYMBOL &&
        tokenizer.symbol() == '(') {
        readSymbol('(');
        getNextToken();
        vmWriter.writePush(Segment::POINTER, 0);
        compileExpressionList();
        readSymbol(')');
        vmWriter.writeCall(nowClassName + "." + callName, argNum + 1);
    } else {
        readSymbol('.');
        getNextToken();
        readIdentifier();
        string subroutineName = tokenizer.identifier();
        bool buildIn = true;
        if (symbolTabel.kindOf(callName) != Kind::NONE)
            vmWriter.writePush(kind2Segment(symbolTabel.kindOf(callName)),
                               symbolTabel.indexOf(callName)),
                buildIn = false;

        getNextToken();
        readSymbol('(');
        getNextToken();
        compileExpressionList();
        readSymbol(')');
        if (!buildIn)
            vmWriter.writeCall(
                symbolTabel.typeOf(callName) + "." + subroutineName,
                argNum + 1);
        else
            vmWriter.writeCall(callName + "." + subroutineName, argNum);
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
