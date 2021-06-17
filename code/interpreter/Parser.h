#ifndef _PARSER_H_
#define _PARSER_H_
#include<deque>
#include<vector>
using namespace std;
class Parser{
public:
    Parser(){}
    bool input(string ins);            //有输入就往里扔，能运行就会执行
    void flushBuffer();
private:
    deque<string> buffer;              //用于暂时存储分号前的string
    void decode(vector<string> args);
    void Create_table(vector<string> args);
    void Drop_table(vector<string> args);
    void Create_index(vector<string> args);
    void Drop_index(vector<string> args);
    void Select(vector<string> args);
    void Selectpart(vector<string> args);
    void Delete(vector<string> args);
    void Insert(vector<string> args);
    void Execfile(vector<string> args);
};
#endif 