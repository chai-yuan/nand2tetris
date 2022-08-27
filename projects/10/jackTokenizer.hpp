#ifndef JACKTOKENIZER_H
#define JACKTOKENIZER_H

#include <fstream>
#include <iostream>
#include <string>
#include "globalDefine.h"
using namespace std;

class Tokenizer {
   public:
    Tokenizer(istream& inputStream);
    bool hasMoreTokens();
    bool advance();
    TokenType tokenType();  // 当前字元的类型
    Keyword keyword();      // 当前字元的关键字
    char symbol();          // 返回当前字元的符号
    string identifier();    // 返回当前字元的标识符
    int intVal();
    string stringVal();

   public:
    istream& inputStream;
    string currentLine, token;
    string::iterator it;
    int linePos;

    bool readNextLine();
    bool nextValidLine();
    bool nextValidChar();
    bool skipComments();
    bool isKeyword(string s);
    bool isSymbol(char c);
    bool isStringConst(string tok);
    bool isIntConst(string tok);
    bool isIdentifier(string tok);
};

Tokenizer::Tokenizer(istream& inputStream) : inputStream(inputStream) {
    readNextLine();
}
bool Tokenizer::advance() {
    if (!nextValidChar())
        return false;
    token.clear();

    if (isSymbol(*it)) {  // 符号
        token.append(1, *it);
        ++it;
        return true;
    }
    // 字符串
    if (*it == '"')
        do {
            token.append(1, *it);
            it++;
        } while (it != currentLine.end() and *it != '"');

    // 其他符号
    while (it != currentLine.end() and !isspace(*it) and !isSymbol(*it)) {
        token.append(1, *it);
        ++it;
    }

    return true;
}
TokenType Tokenizer::tokenType() {
    if (isKeyword(token))
        return TokenType::kKEYWORD;
    if (isSymbol(token[0]))
        return TokenType::kSYMBOL;
    if (isStringConst(token))
        return TokenType::kSTRING_CONST;
    if (isIntConst(token))
        return TokenType::kINT_CONST;
    if (isIdentifier(token))
        return TokenType::kIDENTIFIER;
    return TokenType::kUNKNOWN;
}
Keyword Tokenizer::keyword() {
    if (token == "class")
        return Keyword::kCLASS;
    if (token == "method")
        return Keyword::kMETHOD;
    if (token == "function")
        return Keyword::kFUNCTION;
    if (token == "constructor")
        return Keyword::kCONSTRUCTOR;
    if (token == "int")
        return Keyword::kINT;
    if (token == "boolean")
        return Keyword::kBOOLEAN;
    if (token == "char")
        return Keyword::kCHAR;
    if (token == "void")
        return Keyword::kVOID;
    if (token == "var")
        return Keyword::kVAR;
    if (token == "static")
        return Keyword::kSTATIC;
    if (token == "field")
        return Keyword::kFIELD;
    if (token == "let")
        return Keyword::kLET;
    if (token == "do")
        return Keyword::kDO;
    if (token == "if")
        return Keyword::kIF;
    if (token == "else")
        return Keyword::kELSE;
    if (token == "while")
        return Keyword::kWHILE;
    if (token == "return")
        return Keyword::kRETURN;
    if (token == "true")
        return Keyword::kTRUE;
    if (token == "false")
        return Keyword::kFALSE;
    if (token == "null")
        return Keyword::kNULL;

    return Keyword::kTHIS;
}
char Tokenizer::symbol() {
    return token[0];
}
string Tokenizer::identifier() {
    return token;
}
int Tokenizer::intVal() {
    return stoi(this->token);
}
string Tokenizer::stringVal() {
    string ret;
    for (int i = 1; i < this->token.size() - 1; i++)
        ret.append(1, token[i]);
    return ret;
}

bool Tokenizer::readNextLine() {
    if (getline(inputStream, currentLine)) {
        linePos++;
        it = currentLine.begin();
        return true;
    }
    return false;
}
bool Tokenizer::nextValidLine() {
    if (!readNextLine())
        return false;
    while (true) {
        if (currentLine.empty() or it == currentLine.end()) {
            if (!readNextLine())
                return false;
        } else
            break;
    }
    return true;
}
bool Tokenizer::nextValidChar() {
    while (isspace(*it) or it == currentLine.end()) {
        if (*it == '\n' or it == currentLine.end()) {
            if (!nextValidLine())
                return false;
        } else
            it++;
    }
    if (!skipComments())
        return false;
    return true;
}
bool Tokenizer::skipComments() {
    if (*it != '/')
        return true;
    ++it;
    if (*it == '/')
        return nextValidLine() && nextValidChar();
    // 多行注释
    if (*it == '*') {
        while (true) {
            int dist = distance(currentLine.begin(), it);
            size_t pos = currentLine.find("*/", dist);

            if (pos != string::npos) {
                it = currentLine.begin() + pos + 2;
                return nextValidChar();
            }

            if (!nextValidLine())
                return false;
        }
    }
    --it;
    return true;
}
bool Tokenizer::isKeyword(string s) {
    return jackKeyword.count(s);
}
bool Tokenizer::isSymbol(char c) {
    return jackSymbol.count(c);
}
bool Tokenizer::isStringConst(string tok) {
    if (tok.front() == '\"' && tok.back() == '\"') {
        string::const_iterator it = tok.begin();
        while (it != tok.end() && *it != '\n')
            ++it;
        return it == tok.end();
    }
    return false;
}
bool Tokenizer::isIntConst(string tok) {
    string::const_iterator it = tok.begin();
    while (it != tok.end() && isdigit(*it))
        ++it;
    if (!tok.empty() && it == tok.end()) {
        int num = stoi(tok);
        return true;
    }
    return false;
}
bool Tokenizer::isIdentifier(string tok) {
    if (isdigit(tok.front()))
        return false;
    string::const_iterator it = tok.begin();
    while (it != tok.end() && (isalpha(*it) || isdigit(*it) || *it == '_'))
        ++it;
    return it == tok.end();
}

#endif
