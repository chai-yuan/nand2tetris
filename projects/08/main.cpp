#include <bits/stdc++.h>
#include "fileHandler.hpp"
#include "vmTranslator.hpp"
using namespace std;

int main(int argc, char** argv) {
    // if (FileHandler::isDir(inputName))
    //     FileHandler::getFileFromDir(inputName, ".vm");

    if (argc == 1) {
        cerr << "usage: main fileName.vm|directoryName" << endl;
        return 1;
    }

    // string inputName(argv[1]);

    string inputName(argv[1]), outputName;
    vector<string> inputNameList;
    if (FileHandler::isDir(inputName)) {
        if (inputName.back() != '/')  // 规范格式，方便处理
            inputName += "/";

        outputName = inputName + "main.asm";
        inputNameList = FileHandler::getFileFromDir(inputName, ".vm");
        for (auto& name : inputNameList)
            name = inputName + name;
    } else if (FileHandler::isFile(inputName)) {
        inputNameList.push_back(inputName);
        outputName = "main.asm";
    } else {
        cerr << "error: invalid input" << endl;
        return 1;
    }

    ofstream outputFile(outputName);
    if (!outputFile.good()) {
        cerr << "error: unable to open file" << endl;
        return 1;
    }

    for (auto name : inputNameList) {
        cout << "Start translating:" << name << endl;
        ifstream inputFile(name);
        vmTranslator vm(
            inputFile, outputFile,
            FileHandler::changeExtension(FileHandler::removePath(name), ""));
        vm.translate();
    }
    outputFile.close();

    return 0;
}