#include <bits/stdc++.h>
#include "fileHandler.hpp"
#include "jackAnalyzer.hpp"
using namespace std;

int main(int args, char** argv) {
    string inputName("./Square/");
    vector<string> inputNameList, outputNameList;

    if (FileHandler::isDir(inputName)) {
        auto nameList = FileHandler::getFileFromDir(inputName, "jack");
        for (auto name : nameList)
            inputNameList.push_back(inputName + name);
    } else if (FileHandler::isFile(inputName)) {
        inputNameList.push_back(inputName);
    }

    for (auto iName : inputNameList) {
        outputNameList.push_back(FileHandler::changeExtension(iName, ".txml"));
    }

    Analyzer analyzer(inputNameList, outputNameList);
    analyzer.analyzerCode();

    return 0;
}