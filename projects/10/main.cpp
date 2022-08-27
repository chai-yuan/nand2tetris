#include <bits/stdc++.h>
#include "fileHandler.hpp"
#include "jackAnalyzer.hpp"
using namespace std;

int main(int args, char** argv) {
    string inputName("./ArrayTest/Main.jack");
    string outputName = FileHandler::changeExtension(inputName, ".txml");
    analyzer analyzer({inputName}, outputName);
    analyzer.analyzerCode();

    return 0;
}