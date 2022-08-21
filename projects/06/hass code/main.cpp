#include <bits/stdc++.h>
#include <unistd.h>
#include "assembler.hpp"
#include "fileHandler.hpp"
using namespace std;

// 获得详细参数
bool getFlags(int argc, char** argv, bool& symTable) {
    char c;
    symTable = false;
    while ((c = getopt(argc, argv, "vVt")) != -1) {
        switch (c) {
            case 't':
                symTable = true;
                break;

            case '?':
                return 1;
        }
    }

    return true;
}
// 判断是否是asm文件
bool isAsmFile(string input) {
    if (!FileHandler::isFile(input)) {
        cerr << "error: input \"" << input << "\"is not a file" << endl;
        return false;
    }
    if (!FileHandler::hasExtension(input, ".asm")) {
        cerr << "error: input file must have .asm extension" << endl;
        return false;
    }
    return true;
}

int main(int argc, char** argv) {
    if (argc == 1) {  // 输入参数缺失
        cerr << "usage: "
             << "hass"
             << " [-v|-V] input-filename.asm" << endl;
        cerr << "       -t output symbol table" << endl;
        return 1;
    }

    bool verbose, veryVerbose, symTable;
    if (!getFlags(argc, argv, symTable))
        return 1;

    string inputName(argv[argc - 1]);
    if (!isAsmFile(inputName))
        return 1;

    ifstream inputFile(inputName);  // 打开文件
    if (!inputFile.good()) {
        cerr << "error: unable to open input stream" << endl;
        return 1;
    }
    string outputName(FileHandler::changeExtension(inputName, ".hack"));
    ofstream outputFile(outputName);  // 创建输出文件
    if (!outputFile.good()) {
        cerr << "error: unable to open output stream" << endl;
        return 1;
    }

    // 进行汇编操作
    assembler hass(inputFile, inputName, outputFile, outputName, verbose,
                   veryVerbose);
    hass.assemble();

    // if (symTable) {
    //     string symOutputName(
    //         FileHandler::changeExtension(inputName, "-symbols"));
    //     ofstream symOutputFile(symOutputName);
    //     if (!symOutputFile.good()) {
    //         cerr << "error: unable to open output stream" << endl;
    //         return 1;
    //     }
    //     hass.outputSymbolTable(symOutputFile);
    //     symOutputFile.close();
    //     if (verbose)
    //         cout << "symbol table output: " << symOutputName << endl;
    // }

    if (inputFile.is_open())
        inputFile.close();
    if (outputFile.is_open())
        outputFile.close();
    return 0;
}