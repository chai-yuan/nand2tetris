#ifndef JACKANALYZER_H
#define JACKANALYZER_H

#include <fstream>
#include <string>
#include <vector>
#include "jackTokenizer.hpp"
using namespace std;

class analyzer {
   public:
    analyzer(vector<string> inputNameList, string outputName);
    void analyzerCode();

   private:
    vector<string> inputNameList;
    string outputName;
};

analyzer::analyzer(vector<string> inputNameList, string outputName)
    : inputNameList(inputNameList), outputName(outputName) {}
void analyzer::analyzerCode() {
    ifstream inputFile(inputNameList[0]);
    ofstream outputFile(outputName);

    tokenizer tokenizer(inputFile);
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
                outputFile << "<integerConstant> " << tokenizer.token
                           << " </integerConstant>" << endl;
                break;
            case TokenType::kSTRING_CONST:
                outputFile << "<stringConstant> " << tokenizer.token
                           << " </stringConstant>" << endl;
                break;
            case TokenType::kIDENTIFIER:
                outputFile << "<identifier> " << tokenizer.identifier()
                           << " </identifier>" << endl;
                break;
        }
    }
}

#endif