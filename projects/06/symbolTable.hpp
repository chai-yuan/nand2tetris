#ifndef SYMBOL_TABLE_INCLUDED_H
#define SYMBOL_TABLE_INCLUDED_H

#include <map>
#include <string>
using namespace std;

class symbolTable {
   public:
    void loadMap(map<string, int> symbolMap);   // 加载
    void addEntry(string symbol, int address);  // 添加一条
    bool contains(string symbol);               // 判断是否含有
    int getAddress(string symbol);              // 获得地址
    map<string, int> getTable();                // 返回符号表

   private:
    map<string, int> table; /**< Symbol map. */
};

void symbolTable::loadMap(map<string, int> symbolMap) {
    table.clear();
    table = symbolMap;
}
void symbolTable::addEntry(string symbol, int address) {
    table[symbol] = address;
}
bool symbolTable::contains(string symbol) {
    map<string, int>::iterator it = table.find(symbol);
    return it != table.end() ? true : false;
}
int symbolTable::getAddress(string symbol) {
    map<string, int>::iterator it = table.find(symbol);
    return it != table.end() ? it->second : -1;
}
map<string, int> symbolTable::getTable() {
    return table;
}

#endif
