#ifndef GLOBAL_DE_H
#define GLOBAL_DE_H

#include <bits/stdc++.h>
using namespace std;

enum class TokenType {
    kKEYWORD,      /**< a keyword */
    kSYMBOL,       /**< a symbol */
    kIDENTIFIER,   /**< an identifier */
    kINT_CONST,    /**< an integer constante */
    kSTRING_CONST, /**< a string constant */
    kUNKNOWN       /**< unknown type, used to flag an error */
};

enum class Keyword {
    kCLASS,       /**< class */
    kMETHOD,      /**< method */
    kFUNCTION,    /**< function */
    kCONSTRUCTOR, /**< constructor */
    kINT,         /**< int */
    kBOOLEAN,     /**< boolean */
    kCHAR,        /**< char */
    kVOID,        /**< void */
    kVAR,         /**< var */
    kSTATIC,      /**< static */
    kFIELD,       /**< field */
    kLET,         /**< let */
    kDO,          /**< do */
    kIF,          /**< if */
    kELSE,        /**< else */
    kWHILE,       /**< while */
    kRETURN,      /**< return */
    kTRUE,        /**< true */
    kFALSE,       /**< false */
    kNULL,        /**< null */
    kTHIS,        /**< this */
    kUNKNOWN      /**< unknow, used to flag an error */
};

set<string> jackKeyword = {
    "class", "method", "function", "constructor", "int",   "boolean", "char",
    "void",  "var",    "static",   "field",       "let",   "do",      "if",
    "else",  "while",  "return",   "true",        "false", "null",    "this"};

set<char> jackSymbol = {'{', '}', '(', ')', '[', ']', '.', ',', ';', '+',
                        '-', '*', '/', '&', '|', '<', '>', '=', '~'};

#endif