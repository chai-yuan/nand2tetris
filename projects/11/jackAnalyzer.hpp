#ifndef JACKANALYZER_H
#define JACKANALYZER_H

#include <fstream>
#include <string>
#include <vector>
#include "compilationEngine.hpp"
#include "jackTokenizer.hpp"
#include "vmWriter.hpp"
using namespace std;

class Analyzer {
   public:
    Analyzer(vector<string> inputNameList, vector<string> outputNameList);
    void analyzerCode();

   private:
    vector<string> inputNameList;
    vector<string> outputNameList;
};

Analyzer::Analyzer(vector<string> inputNameList, vector<string> outputNameList)
    : inputNameList(inputNameList), outputNameList(outputNameList) {}
void Analyzer::analyzerCode() {
    for (int i = 0; i < inputNameList.size(); i++) {
        ifstream inputFile(inputNameList[i]);
        ofstream outputFile(outputNameList[i]);

        Tokenizer tokenizer(inputFile);
        VMWriter vmWriter(outputFile);
        CompilationEngine engine(tokenizer, vmWriter);
        engine.compileClass();

        inputFile.close();
        outputFile.close();
    }
}

#endif