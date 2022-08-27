#ifndef JACKANALYZER_H
#define JACKANALYZER_H

#include <fstream>
#include <string>
#include <vector>
// #include "compilationEngine.hpp"
#include "jackTokenizer.hpp"
using namespace std;

class Analyzer {
   public:
    Analyzer(vector<string> inputNameList, string outputName);
    void analyzerCode();

   private:
    vector<string> inputNameList;
    string outputName;
};

Analyzer::Analyzer(vector<string> inputNameList, string outputName)
    : inputNameList(inputNameList), outputName(outputName) {}
void Analyzer::analyzerCode() {
    ifstream inputFile(inputNameList[0]);
    ofstream outputFile(outputName);

    Tokenizer tokenizer(inputFile);
    while (tokenizer.advance()) {
        switch (tokenizer.tokenType()) {
            case TokenType::kKEYWORD:
                outputFile << "<keyword> " << tokenizer.token << " </keyword>"
                           << endl;
                break;
            case TokenType::kSYMBOL:
                outputFile << "<symbol> " << tokenizer.symbol() << " </symbol>"
                           << endl;
                break;
            case TokenType::kINT_CONST:
                outputFile << "<integerConstant> " << tokenizer.intVal()
                           << " </integerConstant>" << endl;
                break;
            case TokenType::kSTRING_CONST:
                outputFile << "<stringConstant> " << tokenizer.stringVal()
                           << " </stringConstant>" << endl;
                break;
            case TokenType::kIDENTIFIER:
                outputFile << "<identifier> " << tokenizer.identifier()
                           << " </identifier>" << endl;
                break;
        }
    }

    // outputFile.close();
}

#endif