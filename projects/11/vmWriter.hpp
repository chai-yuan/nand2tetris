#ifndef VM_WRITER_H
#define VM_WRITER_H

#include <iostream>
using namespace std;

enum class Segment { CONST, ARG, LOCAL, STATIC, THIS, THAT, POINTER, TEMP };
enum class Command { ADD, SUB, NEG, EQ, GT, LT, AND, OR, NOT };

class vmWriter {
   public:
    vmWriter(ostream& outputStream);
    void writePush(Segment segment, int index);
    void writePop(Segment segment, int index);
    void writeArithmetic(Command command);
    void writeLabel(string label);
    void writeGoto(string label);
    void writeIf(string label);
    void writeCall(string name, int nArgs);
    void writeFunction(string name, int nLocals);
    void writeReturn();

   private:
    ostream& outputStream;
};

vmWriter::vmWriter(ostream& outputStream) : outputStream(outputStream) {}
void vmWriter::writePush(Segment segment, int index) {
    outputStream << "push " << segment2String(segment) << " " << index << endl;
}
void vmWriter::writePop(Segment segment, int index) {
    outputStream << "pop " << segment2String(segment) << " " << index << endl;
}
void vmWriter::writeArithmetic(Command command) {
    outputStream << command2String(command) << endl;
}
void vmWriter::writeLabel(string label) {
    outputStream << "label " << label << endl;
}
void vmWriter::writeGoto(string label) {
    outputStream << "goto " << label << endl;
}
void vmWriter::writeIf(string label) {
    outputStream << "if-goto " << label << endl;
}
void vmWriter::writeCall(string name, int nArgs) {
    outputStream << "call " << name << " " << nArgs << endl;
}
void vmWriter::writeFunction(string name, int nLocals) {
    outputStream << "function " << name << " " << nLocals << endl;
}
void vmWriter::writeReturn() {
    outputStream << "return" << endl;
}

string segment2String(Segment seg) {
    switch (seg) {
        case Segment::CONST:
            return "constant";
        case Segment::ARG:
            return "argument";
        case Segment::LOCAL:
            return "local";
        case Segment::STATIC:
            return "static";
        case Segment::THIS:
            return "this";
        case Segment::THAT:
            return "that";
        case Segment::POINTER:
            return "pointer";
        case Segment::TEMP:
            return "temp";
    }
}
string command2String(Command com) {
    switch (com) {
        case Command::ADD:
            return "add";
        case Command::SUB:
            return "sub";
        case Command::NEG:
            return "neg";
        case Command::EQ:
            return "eq";
        case Command::GT:
            return "gt";
        case Command::LT:
            return "lt";
        case Command::AND:
            return "and";
        case Command::OR:
            return "or";
        case Command::NOT:
            return "not";
    }
}
#endif
