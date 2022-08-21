#ifndef ASSEMBLER_INCLUDED_H
#define ASSEMBLER_INCLUDED_H

#include <iostream>
#include <string>
#include "code.hpp"
#include "parser.hpp"
#include "symbolTable.hpp"

using namespace std;

class assembler {
   public:
    assembler(istream& inputStream,
              string inputName,
              ostream& outputStream,
              string outputName);
    // 执行汇编操作
    void assemble();
    void assembleFirstPass();   // 第一遍汇编
    void assembleSecondPass();  // 第二遍汇编

   private:
    istream& inputStream;
    ostream& outputStream;
    string inputName;
    string outputName;
    string bin; /*最后一条汇编指令的二进制字符串 */

    parser parser;           /*要读取/解析输入文件 */
    code code;               /*转换为代码 */
    symbolTable symbolTable; /*符号表 */

    void assembleACommand();   // 将A指令写入文件
    void assembleCCommand();   // 将C指令写入文件
    void PredefinedSymbols();  // 预定义符号
};

assembler::assembler(istream& inputStream,
                     string inputName,
                     ostream& outputStream,
                     string outputName)
    : inputStream(inputStream),
      outputStream(outputStream),
      inputName(inputName),
      outputName(outputName),
      parser(inputStream) {}
void assembler::assemble() {
    PredefinedSymbols();  // 处理预定义符号
    assembleFirstPass();
    assembleSecondPass();
}
void assembler::assembleFirstPass() {
    int num = 0;
    string symbol;

    while (parser.advance()) {
        switch (parser.commandType()) {
            case HasmCommandType::L_COMMAND:
                symbol = parser.symbol();
                symbolTable.addEntry(symbol, num);
                break;
            default:
                num++;
        }
    }
}
void assembler::assembleSecondPass() {
    parser.reset();  // 重置

    while (parser.advance()) {
        switch (parser.commandType()) {
            case HasmCommandType::A_COMMAND:
                assembleACommand();
                break;
            case HasmCommandType::C_COMMAND:
                assembleCCommand();
                break;
            case HasmCommandType::L_COMMAND:
                break;
        }
    }
}

void assembler::assembleACommand() {
    static int RAMadr = 16;
    string symbol = parser.symbol();
    int val;
    if (isdigit(symbol.front())) {
        val = stoi(parser.symbol());
    } else {
        if (symbolTable.contains(symbol)) {
            val = symbolTable.getAddress(symbol);
        } else {
            symbolTable.addEntry(symbol, RAMadr);
            val = RAMadr++;
        }
    }
    bin = bitset<16>(val).to_string();
    outputStream << bin << endl;
}
void assembler::assembleCCommand() {
    string dest = parser.dest();
    string comp = parser.comp();
    string jump = parser.jump();
    unsigned int cc = 0;  // c_command

    cc = code.dest(dest);
    cc |= code.comp(comp);
    cc |= code.jump(jump);
    cc |= 0b1110000000000000;

    bin = bitset<16>(cc).to_string();
    outputStream << bin << endl;
}
void assembler::PredefinedSymbols() {
    symbolTable.addEntry("SP", 0x0000);
    symbolTable.addEntry("LCL", 0x0001);
    symbolTable.addEntry("ARG", 0x0002);
    symbolTable.addEntry("THIS", 0x0003);
    symbolTable.addEntry("THAT", 0x0004);

    for (int i = 0; i < 16; i++) {
        string reg("R");
        reg.append(to_string(i));
        symbolTable.addEntry(reg, i);
    }

    symbolTable.addEntry("SCREEN", 0x4000);
    symbolTable.addEntry("KBD", 0x6000);
}
#endif  // ASSEMBLER_INCLUDED_H
