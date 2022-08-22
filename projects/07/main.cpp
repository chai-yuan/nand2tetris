#include <bits/stdc++.h>
#include "fileHandler.hpp"
#include "vmTranslator.hpp"
using namespace std;

int main(int argc, char** argv) {
    if (argc == 1) {
        cerr << "usage: vm2hack fileName.vm|directoryName" << endl;
        return 1;
    }

    string inputName(argv[0]);
    if (!FileHandler::isFile(inputName) and !FileHandler::isDir(inputName)) {
        cerr << "error: invalid input" << endl;
        return 1;
    }
    if (FileHandler::isDir(inputName))  // 如果是目录，去除目录索引
        inputName = FileHandler::removePath(inputName);

    string outputName(FileHandler::changeExtension(inputName, ".asm"));
    ofstream outputFile(outputName);

    if (!outputFile.good()) {
        cerr << "error: unable to open file" << endl;
        return 1;
    }

    vmTranslator vm(inputName, outputFile);
    vm.translate();
    outputFile.close();

    return 0;
}