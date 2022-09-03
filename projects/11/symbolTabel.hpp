#ifndef SYMBOLTABEL_H
#define SYMBOLTABEL_H

#include <map>
#include <string>
#include "globalDefine.h"
using namespace std;
enum class Kind { STATIC, FIELD, ARG, VAR, NONE };
struct symboltabel {
    string type;
    Kind kind;
    int index;
};

class SymbolTabel {
   public:
    SymbolTabel();
    void startSubroutine();
    void define(string name, string type, Kind kind);
    int varCount(Kind kind);
    Kind kindOf(string name);
    string typeOf(string name);
    int indexOf(string name);

   private:
    map<string, symboltabel> classScope, subroutineScope;
    int staticCnt, fieldCnt, argCnt, varCnt;
};

SymbolTabel::SymbolTabel() {
    staticCnt = fieldCnt = argCnt = varCnt = 0;
}

void SymbolTabel::startSubroutine() {
    subroutineScope.clear();
    argCnt = varCnt = 0;
}

void SymbolTabel::define(string name, string type, Kind kind) {
    switch (kind) {
        case Kind::STATIC:
            classScope[name] = {type, kind, staticCnt++};
            break;
        case Kind::FIELD:
            classScope[name] = {type, kind, fieldCnt++};
            break;
        case Kind::ARG:
            subroutineScope[name] = {type, kind, argCnt++};
            break;
        case Kind::VAR:
            subroutineScope[name] = {type, kind, varCnt++};
            break;
        default:
            break;
    }
}

int SymbolTabel::varCount(Kind kind) {
    switch (kind) {
        case Kind::STATIC:
            return staticCnt;
        case Kind::FIELD:
            return fieldCnt;
        case Kind::ARG:
            return argCnt;
        case Kind::VAR:
            return varCnt;
        default:
            return -1;
    }
}

Kind SymbolTabel::kindOf(string name) {
    if (classScope.find(name) != classScope.end())
        return classScope[name].kind;
    else if (subroutineScope.find(name) != subroutineScope.end())
        return subroutineScope[name].kind;
    else
        return Kind::NONE;
}

string SymbolTabel::typeOf(string name) {
    if (classScope.find(name) != classScope.end())
        return classScope[name].type;
    else if (subroutineScope.find(name) != subroutineScope.end())
        return subroutineScope[name].type;
    else
        return "";
}

int SymbolTabel::indexOf(string name) {
    if (classScope.find(name) != classScope.end())
        return classScope[name].index;
    else if (subroutineScope.find(name) != subroutineScope.end())
        return subroutineScope[name].index;
    else
        return -1;
}

Kind keyWord2Kind(Keyword Key) {
    switch (Key) {
        case Keyword::kSTATIC:
            return Kind::STATIC;
        case Keyword::kFIELD:
            return Kind::FIELD;
        case Keyword::kVAR:
            return Kind::VAR;
    }
}

#endif