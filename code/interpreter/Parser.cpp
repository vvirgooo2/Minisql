#include<string>
#include<cstdlib>
#include<deque>
#include<iostream>
#include<vector>
#include"Parser.h"
#include"../SqlDataType.h"
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

//找出表名和各属性，传入API
void Parser::Create_table(vector<string> args){
    //cout<<"test for create table"<<endl;
    // create table tablename ( name int , name float, name char ( length ) , primary key ( name ) )
    string tablename=args.at(2);
    /*test*/
    cout<<"table name: ";
    cout<<tablename<<endl;

    int i=3;
    if(args.at(i++)!="(") cout<<"syntax error!"<<endl;
    vector<attri_type> attris;
    while(1){
        attri_type tattr;
        string name=args.at(i);
        //primary key 定义
        if(name=="primary"){
            if(args.at(i+1)=="key"){
                auto itr=attris.begin();
                for(;itr!=attris.end();itr++){
                    if(itr->attri_name==args.at(i+3)){
                        itr->primary=true;
                        break;
                    }
                }
            }
            i+=5;
        }
        //属性定义
        else{
            //store name
            tattr.attri_name=name;
            i++; 
            //type
            //store type
            string type=args.at(i);
            if(type=="int") tattr.type=AType::Integer;
            else if(type=="float") tattr.type=AType::Float;
            else if(type=="char") {
                tattr.type=AType::String;
                if(args.at(i+1)=="("&&args.at(i+3)==")"){
                    string length=args.at(i+2);
                    stringstream ss;
                    ss<<length;
                    ss>>tattr.char_sz;
                    i=i+3;
                }
                else{ cout<<"syntax error!"<<endl; return; }
            }
            else { cout<<"Unknown type"<<endl; return;  }
            
            if(args.at(i+1)=="unique"){
                tattr.unique=true;
                i++;
            }
            attris.push_back(tattr);
            i++;
        }
        if(args.at(i)!=","&&args.at(i)!=")") { cout<<"syntax error!"<<endl; return; }
        if(args.at(i)==")") break;
        else i++;
    }
    
    //debug part
    for(auto itr=attris.begin();itr!=attris.end();itr++){
        cout<<itr->attri_name<<" ";
        switch(itr->type){
            case AType::Integer: cout<<"int  "; break;
            case AType::Float: cout<<"float  " ; break;
            case AType::String: cout<<"String  "; break;

        }
        if(itr->char_sz!=0) cout<<itr->char_sz;
        if(itr->primary) cout<<"  primary ";
        if(itr->unique) cout<<"  unique  ";
        cout<<endl;
    }
    /*  调用API,传入表名和各属性
        API_create_table(tablename,attris);
    */

    
}

void Parser::Drop_table(vector<string> args){
    //drop table tablename
    string tablename;
    tablename=args.at(2);
    if(args.size()>3) { cout<<"One table once!"<<endl; return; }
    else {
        cout<<"API drop "<<tablename<<endl;
        /* 调用API，只有一个表名作参数
        API_drop_table(tablename);
        */
    }
}

void Parser::Create_index(vector<string> args){
    
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