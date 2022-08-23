#ifndef PARSER_H
#define PARSER_H

enum class VmCommandType {
    C_ARITHMETIC, /**< Arithmetic command. */
    C_PUSH,       /**< Push command. */
    C_POP,        /**< Pop command. */
    C_LABEL,      /**< Label command. */
    C_GOTO,       /**< Goto command. */
    C_IF,         /**< If Goto command. */
    C_FUNCTION,   /**< Function command. */
    C_RETURN,     /**< Return command. */
    C_CALL        /**< Call command. */
};

#include <string.h>
#include <fstream>
#include <vector>
using namespace std;

class parser {
   public:
    parser(istream& inputStream) : inputStream(inputStream){};
    bool hasMoreCommands();
    bool advance();
    VmCommandType commandType();
    string arg1();
    int arg2();

   private:
    istream& inputStream;
    string currentLine;
    int linePos;
    vector<string> cmds;

    void clearCurrentComments();  // 清除当前行的注释
    bool nextValidLine();         // 更新到下一个有效行
    void trimLine();              // 清除多余的空格和制表符
};

bool parser::hasMoreCommands() {
    if (getline(inputStream, currentLine)) {
        linePos++;
        return true;
    }
    return false;
}
bool parser::advance() {
    while (nextValidLine()) {
        cmds.clear();
        auto it = currentLine.begin();
        while (it != currentLine.end()) {
            string tmpGet;
            while (isspace(*it) and it != currentLine.end())
                it++;

            while (!isspace(*it) and it != currentLine.end()) {
                tmpGet.append(1, *it);
                it++;
            }

            if (!tmpGet.empty())
                cmds.push_back(tmpGet);
        }
        return true;
    }
    return false;
}
VmCommandType parser::commandType() {
    if (cmds.front() == "push")
        return VmCommandType::C_PUSH;
    if (cmds.front() == "pop")
        return VmCommandType::C_POP;
    return VmCommandType::C_ARITHMETIC;
}
string parser::arg1() {
    if (commandType() == VmCommandType::C_ARITHMETIC)
        return cmds.front();
    return cmds.at(1);
}
int parser::arg2() {
    return stoi(cmds.at(2));
}

bool parser::nextValidLine() {
    while (true) {
        if (!hasMoreCommands())
            return false;

        clearCurrentComments();
        trimLine();

        if (currentLine.size() > 1)
            return true;
    }
}
void parser::clearCurrentComments() {
    if (currentLine.find("//") != string::npos)
        currentLine.erase(currentLine.find("//"));
}
void parser::trimLine() {
    currentLine.erase(0, currentLine.find_first_not_of(' '));
    currentLine.erase(0, currentLine.find_first_not_of('\t'));
    currentLine.erase(currentLine.find_last_not_of(' ') + 1);
    currentLine.erase(currentLine.find_last_not_of('\t') + 1);
}

#endif