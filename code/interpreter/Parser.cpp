#include<string>
#include<deque>
#include<iostream>
#include<vector>
#include"Parser.h"
#include<iomanip>
#include<iterator>
#include<sstream>
#include<algorithm>
using namespace std;
bool Parser::input(std::string input){
    // 预处理，所有语句块分开,主要是数学符号,标点
    for(int i=0; i<input.length();i++){
        if(input[i]=='*'||input[i]=='='||input[i]=='<'||input[i]=='>'||input[i]=='.'||input[i]==','
           ||input[i]=='('||input[i]==')'||input[i]==';'){
               if(input[i-1]!=' ') { input.insert(i," "); i++; }
               if(input[i+1]!=' '&&input[i+1]!='\0') { input.insert(i+1," "); i++; }
           }
    }
    //std::cout<<input;
    //每个语句块存一个vector
    istringstream split(input);
    vector<std::string> ins_s((istream_iterator<string>(split)),
                                    istream_iterator<string>());
    buffer.insert(buffer.end(),ins_s.begin(),ins_s.end());
    
    //check ;
    while(true){
       deque<string>::iterator it= find(buffer.begin(), buffer.end(), ";");
       if(it!=buffer.end()){
           int n=it-buffer.begin();
           vector<string> args;
           for(int i=0; i<n; i++){
               args.push_back(buffer.front());
               buffer.pop_front();
           }
           buffer.pop_front();  //;
           if(args.empty()) continue;
           decode(args);
       }
       else break;  //如果没有; 本次先存buffer里面，直到有;
    }
    return buffer.empty();
}

//如果出错，就清空缓存
void Parser::flushBuffer(){
    buffer.clear();
}

//译码，将命令分类
void Parser::decode(vector<string> args){
    if(args.at(0)=="create"&&args.size()>=2&&args.at(1)=="table")      Create_table(args);
    else if(args.at(0)=="drop"&&args.size()>=2&&args.at(1)=="table")   Drop_table(args);
    else if(args.at(0)=="create"&&args.size()>=2&&args.at(1)=="index") Create_index(args);
    else if(args.at(0)=="drop"&&args.size()>=2&&args.at(1)=="index")   Drop_index(args);
    else if(args.at(0)=="select")                      Select(args);
    else if(args.at(0)=="insert")                      Insert(args);
    else if(args.at(0)=="delete")                      Delete(args);
    else if(args.at(0)=="execfile")                    Execfile(args);
    else {
        cout<<"Unknown instruction."<<endl;
        flushBuffer();
        return;
    }
}

void Parser::Create_table(vector<string> args){
    cout<<"test for create table"<<endl;
}

void Parser::Drop_table(vector<string> args){
    cout<<"test for drop table"<<endl;
}

void Parser::Create_index(vector<string> args){
    cout<<"test for create index"<<endl;
}

void Parser::Drop_index(vector<string> args){
    cout<<"test for drop index"<<endl;
}

void Parser::Select(vector<string> args){
    cout<<"test for select"<<endl;
}

void Parser::Insert(vector<string> args){
    cout<<"test for insert"<<endl;
}

void Parser::Delete(vector<string> args){
    cout<<"test for delete"<<endl;
}

void Parser::Execfile(vector<string> args){
    cout<<"test for execfile"<<endl;
}