#ifndef CODE_WRITER_H
#define CODE_WRITER_H

#include <iostream>
#include <string>
#include "parser.hpp"
using namespace std;

class codeWriter {
   public:
    codeWriter(ostream& outputStream, string programName);
    void writeArithmetic(string command);
    void writePushPop(VmCommandType cmd, string segment, int index);
    void writeLabel(string label);  // 需要做函数区别化
    void writeGoto(string label);
    void writeIf(string label);
    void writeCall(string functionName, int argNum);
    void writeReturn();
    void writeFunction(string functionName, int numLocals);
    void writeInit();

   private:
    ostream& outputStream;
    string programName, nowFunctionName;
    int symbolCounter;
    void writeArithmeticAdd();
    void writeArithmeticSub();
    void writeArithmeticNeg();
    void writeArithmeticEq();
    void writeArithmeticGt();
    void writeArithmeticLt();
    void writeArithmeticAnd();
    void writeArithmeticOr();
    void writeArithmeticNot();
    void writePush(string segment, int index);
    void writePop(string segment, int index);
    void write(string code, bool ident = true);
    string registerName(string segment, int index);
    string makeLabel(string label, string type = "");
};

codeWriter::codeWriter(ostream& outputStream, string programName)
    : outputStream(outputStream), programName(programName), symbolCounter(0) {
    nowFunctionName.clear();
}
void codeWriter::writeArithmetic(string command) {
    if (command == "add")
        writeArithmeticAdd();
    else if (command == "sub")
        writeArithmeticSub();
    else if (command == "neg")
        writeArithmeticNeg();
    else if (command == "eq")
        writeArithmeticEq();
    else if (command == "gt")
        writeArithmeticGt();
    else if (command == "lt")
        writeArithmeticLt();
    else if (command == "and")
        writeArithmeticAnd();
    else if (command == "or")
        writeArithmeticOr();
    else
        writeArithmeticNot();
}
void codeWriter::writePushPop(VmCommandType cmd, string segment, int index) {
    if (cmd == VmCommandType::C_PUSH)
        writePush(segment, index);
    else
        writePop(segment, index);
}
void codeWriter::writeLabel(string label) {
    string com("// label " + label);
    write("(" + makeLabel(label) + ")" + com, false);
}
void codeWriter::writeGoto(string label) {
    write("@" + makeLabel(label));
    write("0;JMP");
}
void codeWriter::writeIf(string label) {
    string com("// if-goto " + label);
    write("@SP" + com);
    write("AM=M-1");
    write("D=M");
    write("@" + makeLabel(label));
    write("D;JNE");
}
void codeWriter::writeCall(string functionName, int argNum) {
    symbolCounter++;
    string com("// call " + functionName + " " + to_string(argNum));
    write("@" + makeLabel("RETURN_", to_string(symbolCounter)) +
          com);  // push return-addr
    write("D=A");
    write("@SP");
    write("A=M");
    write("M=D");
    write("@SP");
    write("M=M+1");

    write("@LCL");  // push LCL
    write("D=M");
    write("@SP");
    write("A=M");
    write("M=D");
    write("@SP");
    write("M=M+1");

    write("@ARG");  // push ARG
    write("D=M");
    write("@SP");
    write("A=M");
    write("M=D");
    write("@SP");
    write("M=M+1");

    write("@THIS");  // push THIS
    write("D=M");
    write("@SP");
    write("A=M");
    write("M=D");
    write("@SP");
    write("M=M+1");

    write("@THAT");  // push THAT
    write("D=M");
    write("@SP");
    write("A=M");
    write("M=D");
    write("@SP");
    write("M=M+1");

    write("@" + to_string(argNum));  // ARG = SP-n-5
    write("D=A");
    write("@5");
    write("D=D+A");
    write("@SP");
    write("D=M-D");
    write("@ARG");
    write("M=D");

    write("@SP");  // LCL = SP
    write("D=M");
    write("@LCL");
    write("M=D");

    write("@" + functionName);  // goto f
    write("0;JMP");

    write("(" + makeLabel("RETURN_", to_string(symbolCounter)) + ")",
          false);  // (return-address)
}
void codeWriter::writeReturn() {
    write("@LCL // return");  // FRAME = LCL
    write("D=M");
    write("@R13");  // R13 -> FRAME
    write("M=D");

    write("@5");  // RET = *(FRAME-5)
    write("A=D-A");
    write("D=M");
    write("@R14");  // R14 -> RET
    write("M=D");

    write("@SP");  // *ARG = pop()
    write("AM=M-1");
    write("D=M");
    write("@ARG");
    write("A=M");
    write("M=D");

    write("D=A");  // SP = ARG+1
    write("@SP");
    write("M=D+1");

    write("@R13");  // THAT = *(FRAME-1)
    write("AM=M-1");
    write("D=M");
    write("@THAT");
    write("M=D");

    write("@R13");  // THIS = *(FRAME-2)
    write("AM=M-1");
    write("D=M");
    write("@THIS");
    write("M=D");

    write("@R13");  // ARG = *(FRAME-3)
    write("AM=M-1");
    write("D=M");
    write("@ARG");
    write("M=D");

    write("@R13");  // LCL = *(FRAME-4)
    write("AM=M-1");
    write("D=M");
    write("@LCL");
    write("M=D");

    write("@R14");  // goto RET
    write("A=M");
    write("0;JMP");
}
void codeWriter::writeFunction(string functionName, int numLocals) {
    nowFunctionName = functionName;  // 更新名字用于生成标签

    write("// function : " + functionName + to_string(numLocals));
    write("(" + functionName + ")", false);
    // 循环创建局部变量
    write("@" + to_string(numLocals));
    write("D=A");
    write("@R13");  // temp
    write("M=D");
    write("(" + makeLabel("INIT_LOOP_") + ")", false);
    write("@" + makeLabel("END_INIT_LOOP_"));
    write("D;JEQ");
    write("@0");
    write("D=A");
    write("@SP");
    write("A=M");
    write("M=D");
    write("@SP");
    write("M=M+1");
    write("@R13");
    write("MD=M-1");
    write("@" + makeLabel("INIT_LOOP_"));
    write("0;JMP");
    write("(" + makeLabel("END_INIT_LOOP_") + ")", false);
}
void codeWriter::writeInit() {
    write("@256 // 程序起点");
    write("D=A");
    write("@SP");
    write("M=D");
    writeCall("Sys.init", 0);
    write("(PROGRAM_BEGIN)", false);
    write("@PROGRAM_BEGIN");
    write("0;JMP");
}

void codeWriter::writeArithmeticAdd() {
    write("@SP // add");
    write("AM=M-1");
    write("D=M");
    write("A=A-1");
    write("M=D+M");
}
void codeWriter::writeArithmeticSub() {
    write("@SP // sub");
    write("AM=M-1");
    write("D=M");
    write("A=A-1");
    write("M=M-D");
}
void codeWriter::writeArithmeticNeg() {
    write("@SP // neg");
    write("A=M");
    write("A=A-1");
    write("M=-M");
}
void codeWriter::writeArithmeticEq() {
    string label(makeLabel("Label", to_string(symbolCounter)));
    write("@SP // eq");
    write("AM=M-1");
    write("D=M");
    write("@SP");
    write("AM=M-1");
    write("D=M-D");
    write("@" + label);
    write("D;JEQ");
    write("D=1");
    write("(" + label + ")", false);
    write("D=D-1");
    write("@SP");
    write("A=M");
    write("M=D");
    write("@SP");
    write("M=M+1");
    symbolCounter++;
}
void codeWriter::writeArithmeticGt() {
    string labelTrue(makeLabel("TRUE", to_string(symbolCounter)));
    string labelFalse(makeLabel("FALSE", to_string(symbolCounter)));
    write("@SP // gt");
    write("AM=M-1");
    write("D=M");
    write("@SP");
    write("AM=M-1");
    write("D=M-D");
    write("@" + labelTrue);
    write("D;JGT");
    write("D=0");
    write("@" + labelFalse);
    write("0;JMP");
    write("(" + labelTrue + ")", false);
    write("D=-1");
    write("(" + labelFalse + ")", false);
    write("@SP");
    write("A=M");
    write("M=D");
    write("@SP");
    write("M=M+1");
    symbolCounter++;
}
void codeWriter::writeArithmeticLt() {
    string labelTrue(makeLabel("TRUE", to_string(symbolCounter)));
    string labelFalse(makeLabel("FALSE", to_string(symbolCounter)));

    write("@SP // lt");
    write("AM=M-1");
    write("D=M");
    write("@SP");
    write("AM=M-1");
    write("D=M-D");
    write("@" + labelTrue);
    write("D;JLT");
    write("D=0");
    write("@" + labelFalse);
    write("0;JMP");
    write("(" + labelTrue + ")", false);
    write("D=-1");
    write("(" + labelFalse + ")", false);
    write("@SP");
    write("A=M");
    write("M=D");
    write("@SP");
    write("M=M+1");
    symbolCounter++;
}
void codeWriter::writeArithmeticAnd() {
    write("@SP // and");
    write("AM=M-1");
    write("D=M");
    write("A=A-1");
    write("M=D&M");
}
void codeWriter::writeArithmeticOr() {
    write("@SP // or");
    write("AM=M-1");
    write("D=M");
    write("A=A-1");
    write("M=D|M");
}
void codeWriter::writeArithmeticNot() {
    write("@SP // not");
    write("A=M");
    write("A=A-1");
    write("M=!M");
}
void codeWriter::writePush(string segment, int index) {
    string indexStr = to_string(index);
    string registerStr = registerName(segment, index);

    if (segment == "constant") {  // push constant

        write("@" + indexStr + " // push " + segment + " " + indexStr);
        write("D=A");
        write("@SP");
        write("A=M");
        write("M=D");
        write("@SP");
        write("M=M+1");

    } else if (segment == "static" || segment == "temp" ||
               segment == "pointer") {  // push static or temp

        write("@" + registerStr + " // push " + segment + " " + indexStr);
        write("D=M");
        write("@SP");
        write("A=M");
        write("M=D");
        write("@SP");
        write("M=M+1");

    } else {  // all other segments

        write("@" + registerStr + " // push " + segment + " " + indexStr);
        write("D=M");
        write("@" + indexStr);
        write("A=D+A");
        write("D=M");
        write("@SP");
        write("A=M");
        write("M=D");
        write("@SP");
        write("M=M+1");
    }
}
void codeWriter::writePop(string segment, int index) {
    string indexStr = to_string(index);
    string registerStr = registerName(segment, index);

    // pop static, temp or pointer
    if (segment == "static" || segment == "temp" || segment == "pointer") {
        write("@SP // pop " + segment + " " + indexStr);
        write("AM=M-1");
        write("D=M");
        write("@" + registerStr);
        write("M=D");
    } else {  // all other segments
        write("@" + registerStr + " // pop " + segment + " " + indexStr);
        write("D=M");
        write("@" + indexStr);
        write("D=D+A");
        write("@R13");
        write("M=D");
        write("@SP");
        write("AM=M-1");
        write("D=M");
        write("@R13");
        write("A=M");
        write("M=D");
    }
}
void codeWriter::write(string code, bool ident) {
    if (ident)
        outputStream << "\t";
    outputStream << code << endl;
}
string codeWriter::registerName(string segment, int index) {
    if (segment == "local")
        return "LCL";
    if (segment == "argument")
        return "ARG";
    if (segment == "this")
        return "THIS";
    if (segment == "that")
        return "THAT";
    if (segment == "pointer")
        return "R" + to_string(3 + index);
    if (segment == "temp")
        return "R" + to_string(5 + index);

    return programName + "." + to_string(index);  // else it is static
}

string codeWriter::makeLabel(string label, string type) {
    return programName + "$" + nowFunctionName + "$" + label + type;
}

#endif
