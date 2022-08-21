#ifndef ASSEMBLER_INCLUDED_H
#define ASSEMBLER_INCLUDED_H

#include <iostream>
#include <string>
#include "code.hpp"
#include "parser.hpp"

using namespace std;

class assembler {
   public:
    assembler(istream& inputStream,
              string inputName,
              ostream& outputStream,
              string outputName,
              bool verbose = false,
              bool veryVerbose = false);
    // 执行汇编操作
    void assemble();

   private:
    istream& inputStream;
    ostream& outputStream;
    string inputName;
    string outputName;
    string bin; /*最后一条汇编指令的二进制字符串 */

    parser parser; /*要读取/解析输入文件 */
    code code;     /**< To translate the input from the parser. */

    bool verbose;     /*详细模式的标志*/
    bool veryVerbose; /*“非常详细”模式的标志 */

    const string assemblerTitle;    /**< Assembler title. */
    const string assemblerSubtitle; /**< Assembler subtitle. */
    const string assemblerVersion;  /**< Assembler version. */

    void assembleACommand();  // 将A指令写入文件
    void assembleCCommand();  // 将C指令写入文件
};

assembler::assembler(istream& inputStream,
                     string inputName,
                     ostream& outputStream,
                     string outputName,
                     bool verbose,
                     bool veryVerbose)
    : inputStream(inputStream),
      outputStream(outputStream),
      inputName(inputName),
      outputName(outputName),
      parser(inputStream),
      verbose(verbose),
      veryVerbose(veryVerbose),
      assemblerTitle("hass"),
      assemblerSubtitle(
          "hack assembler symbol-less version (nand2tetris, chap. 6)"),
      assemblerVersion("1.0") {}
void assembler::assemble() {
    while (parser.advance()) {
        switch (parser.commandType()) {
            case HasmCommandType::A_COMMAND:
                assembleACommand();
                break;
            case HasmCommandType::C_COMMAND:
                assembleCCommand();
            case HasmCommandType::L_COMMAND:
                break;
        }
    }
    if (verbose)
        cout << "done" << endl << endl;
}

void assembler::assembleACommand() {
    bin = bitset<16>(stoi(parser.symbol())).to_string();
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

#endif  // ASSEMBLER_INCLUDED_H
