#ifndef VM_TRANSLATOR_H
#define VM_TRANSLATOR_H

#include <iostream>
#include <string>
#include "codeWriter.hpp"
#include "parser.hpp"
using namespace std;

class vmTranslator {
   public:
    vmTranslator(istream& inputStream,
                 ostream& outputStream,
                 string programName);
    void translate();

   private:
    string programName;
    istream& inputStream;
    ostream& outputStream;
};

vmTranslator::vmTranslator(istream& inputStream,
                           ostream& outputStream,
                           string programName)
    : inputStream(inputStream),
      outputStream(outputStream),
      programName(programName) {}

void vmTranslator::translate() {
    parser parser(inputStream);
    codeWriter codeWriter(outputStream, programName);

    codeWriter.writeInit();  // 如果编译全部程序，写程序头，从sys.init开始执行
    while (parser.advance()) {
        if (parser.commandType() == VmCommandType::C_ARITHMETIC) {
            codeWriter.writeArithmetic(parser.arg1());
        } else if (parser.commandType() == VmCommandType::C_LABEL) {
            codeWriter.writeLabel(parser.arg1());
        } else if (parser.commandType() == VmCommandType::C_GOTO) {
            codeWriter.writeGoto(parser.arg1());
        } else if (parser.commandType() == VmCommandType::C_IF) {
            codeWriter.writeIf(parser.arg1());
        } else if (parser.commandType() == VmCommandType::C_FUNCTION) {
            codeWriter.writeFunction(parser.arg1(), parser.arg2());
        } else if (parser.commandType() == VmCommandType::C_CALL) {
            codeWriter.writeCall(parser.arg1(), parser.arg2());
        } else if (parser.commandType() == VmCommandType::C_RETURN) {
            codeWriter.writeReturn();
        } else {
            codeWriter.writePushPop(parser.commandType(), parser.arg1(),
                                    parser.arg2());
        }
    }
}

#endif
