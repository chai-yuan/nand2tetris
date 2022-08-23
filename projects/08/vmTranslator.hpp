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

    while (parser.advance()) {
        if (parser.commandType() == VmCommandType::C_ARITHMETIC) {
            codeWriter.writeArithmetic(parser.arg1());
        } else {
            codeWriter.writePushPop(parser.commandType(), parser.arg1(),
                                    parser.arg2());
        }
    }
}

#endif
