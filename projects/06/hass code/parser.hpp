#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <string>
using namespace std;

enum class HasmCommandType {  // 命令类型
    A_COMMAND,
    C_COMMAND,
    L_COMMAND
};

class parser {
   public:
    parser(istream& inputStream) : inputStream(inputStream), linePos(0) {
        readNextLine();
    };
    bool hasMoreCommands();  // 返回是否还有更多命令
    bool advance();  // 当还有更多命令时，将下一条命令写入command
    HasmCommandType commandType();  // 返回当前命令的类型
    string symbol();  // 返回形如@XXX 或 (XXX)的命令的符号或者十进制值
    string dest();  // 返回当前C指令的dest助记符
    string comp();  // 返回当前C指令的comp助记符
    string jump();  // 返回当前C指令的jump助记符
    void reset();   // 重置

   private:
    istream& inputStream;         // 输入流
    string currentLine, command;  // 当前行和当前命令
    string::iterator it;          // 当前行迭代器指针
    int linePos;                  // 当前行号

    bool readNextLine();   // 读取下一行
    bool nextValidLine();  // 更新下一个有效行
    bool nextValidChar();  // 更新迭代器到下一个有效位
    bool skipComments();   // 跳过注释
};

bool parser::hasMoreCommands() {
    return nextValidChar();
}
bool parser::advance() {
    if (hasMoreCommands()) {
        command.clear();
        while (it != currentLine.end() && !isspace(*it)) {
            command.append(1, *it);
            ++it;
        }
        return true;
    }
    return false;
}
HasmCommandType parser::commandType() {
    if (command.front() == '@')
        return HasmCommandType::A_COMMAND;
    if (command.front() == '(')
        return HasmCommandType::L_COMMAND;
    return HasmCommandType::C_COMMAND;
}
string parser::symbol() {
    if (commandType() == HasmCommandType::A_COMMAND)
        return command.substr(1);
    return command.substr(1, command.size() - 2);
}
string parser::dest() {
    // C_COMMAND dest=comp;jump
    if (command.find('=') != string::npos)
        return command.substr(0, command.find('='));
    return "";
}
string parser::comp() {
    // C_COMMAND dest=comp;jump
    if (command.find('=') == string::npos)  // dest omitted
        return command.substr(0, command.find(';'));
    if (command.find(';') == string::npos)  // jump omitted
        return command.substr(command.find('=') + 1);
    return command.substr(command.find('=') + 1, command.find(';'));
}
string parser::jump() {
    // C_COMMAND dest=comp;jump
    if (command.find(';') != string::npos)
        return command.substr(command.find(';') + 1);
    return "";
}
void parser::reset() {
    inputStream.clear();
    inputStream.seekg(0);
    linePos = 0;
    readNextLine();
}

bool parser::readNextLine() {
    if (getline(inputStream, currentLine)) {
        linePos++;
        it = currentLine.begin();
        return true;
    }
    return false;
}
bool parser::nextValidLine() {
    if (!readNextLine())
        return false;
    while (true) {  // 循环读取，直到读到非空行
        if (currentLine.empty() || it == currentLine.end()) {
            if (!readNextLine())
                return false;
        } else {
            break;
        }
    }
    return true;
}
bool parser::nextValidChar() {
    while (isspace(*it) or it == currentLine.end()) {  // 跳过空格
        if (*it == '\n' or it == currentLine.end()) {
            if (!nextValidLine())
                return false;
        } else
            it++;
    }
    if (!skipComments())  // skip comments here
        return false;
    return true;
}
bool parser::skipComments() {
    if (*it != '/')  // if not char '/' then it can't be a comment
        return true;
    ++it;
    if (*it == '/')  // single line comment
        return nextValidLine() and nextValidChar();

    if (*it == '*') {  // multi line comment
        while (true) {
            int dist = distance(currentLine.begin(), it);
            size_t pos = currentLine.find("*/", dist);
            if (pos != string::npos) {  // 再当前行找到结尾
                it = currentLine.begin() + pos + 2;
                return nextValidChar();  // 返回下一个有效字符
            }
            if (!nextValidLine())
                return false;
        }
    }
    --it;  // if not a comment, go back to previous valid char
    return true;
}
#endif