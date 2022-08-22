#ifndef VM_TRANSLATOR_H
#define VM_TRANSLATOR_H

#include <iostream>
#include <string>
#include "codeWriter.hpp"
#include "parser.hpp"
using namespace std;

class vmTranslator {
   public:
    vmTranslator(string inputName, ostream& outputStream);
    void translate();

   private:
    string inputName;
    ostream& outputStream;
};

vmTranslator::vmTranslator(string inputName, ostream& outputStream)
    : inputName(inputName), outputStream(outputStream) {}

void vmTranslator::translate() {
    codeWriter codeWriter(outputStream, inputName);
}

#endif
