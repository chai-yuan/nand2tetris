#include <bits/stdc++.h>
#include "fileHandler.hpp"
#include "vmTranslator.hpp"
using namespace std;

int main(int argc, char** argv) {
    // string inputName("./FunctionCalls/NestedCall/");
    // if (FileHandler::isDir(inputName))
    //     FileHandler::getFileFromDir(inputName, ".vm");

    if (argc == 1) {
        cerr << "usage: vm2hack fileName.vm|directoryName" << endl;
        return 1;
    }

    string inputName(argv[1]);
    cout << "Start translating:" << inputName << endl;
    if (!FileHandler::isFile(inputName) and !FileHandler::isDir(inputName)) {
        cerr << "error: invalid input" << endl;
        return 1;
    }

    string outputName(FileHandler::changeExtension(inputName, ".asm"));

    ifstream inputFile(inputName);
    ofstream outputFile(outputName);

    if (!outputFile.good()) {
        cerr << "error: unable to open file" << endl;
        return 1;
    }

    vmTranslator vm(inputFile, outputFile, FileHandler::removePath(inputName));
    vm.translate();
    outputFile.close();

    return 0;
}