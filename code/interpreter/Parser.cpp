#include<string>
#include<cstdlib>
#include<deque>
#include<iostream>
#include<vector>
#include"Parser.h"
#include"../SqlDataType.h"
#include"../API/API.h"
#include<iomanip>
#include<iterator>
#include<sstream>
#include<algorithm>
#include<chrono>
#include<fstream>
using namespace std;
//处理输入，遇到分号执行一次
bool Parser::input(std::string input){
    // 预处理，所有语句块分开,主要是数学符号,标点
    for(int i=0; i<(int)input.length();i++){
        if(input[i]=='*'||input[i]=='='||input[i]=='<'||input[i]=='>'||input[i]==','
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
    else if(args.at(0)=="select"&&args.size()>=2&&args.at(1)=="*" )    Select(args);
    else if(args.at(0)=="select"&&args.size()>=2&&args.at(1)!="*" )    Selectpart(args);
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
    try{
    string tablename=args.at(2);
    /*test*/
    cout<<"table name: ";
    cout<<tablename<<endl;

    int i=3;
    if(args.at(i++)!="(") throw std::runtime_error("SYNTAX ERROR: You have an error in your SQL syntax! (create table)");
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
                    tattr.char_sz=stoi(length);
                    i=i+3;
                }
                else{ throw std::runtime_error("SYNTAX ERROR: You have an error in your SQL syntax! (create table)"); }
            }
            else {throw std::runtime_error("SYNTAX ERROR: You have an error in your SQL syntax! (create table)"); }
            
            if(args.at(i+1)=="unique"){
                tattr.unique=true;
                i++;
            }
            attris.push_back(tattr);
            i++;
        }
        if(args.at(i)!=","&&args.at(i)!=")") { throw std::runtime_error("SYNTAX ERROR: You have an error in your SQL syntax! (create table)"); }
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
    /*  调用API,传入表名和各属性  */
    auto start_time = std::chrono::high_resolution_clock::now();
    API_create_table(tablename,attris);
    auto finish_time = std::chrono::high_resolution_clock::now();
    int tempTime = std::chrono::duration_cast<std::chrono::nanoseconds>(finish_time - start_time).count();
    if (tempTime == 0) tempTime = 10;
    std::cerr << "(" << setiosflags(ios::fixed) << setw(9) << setprecision(9) << tempTime * 1e-9 << " s)" << std::endl;
    }
    catch (std::out_of_range) { throw std::runtime_error("SYNTAX ERROR: You have an error in your SQL syntax! (create table)");}
}

//传给API 表名
void Parser::Drop_table(vector<string> args){
    try{
    //drop table tablename
    string tablename;
    tablename=args.at(2);
    if(args.size()>3) { throw std::runtime_error("SYNTAX ERROR: You can only drop one table once");}
    else {
        cout<<"API drop table "<<tablename<<endl;
        //在删除之前测试有没有这张表
        //CatalogManager_exist(tablename);
        /* 调用API，只有一个表名作参数
        
        */
        auto start_time = std::chrono::high_resolution_clock::now();
        API_drop_table(tablename);
        auto finish_time = std::chrono::high_resolution_clock::now();
        int tempTime = std::chrono::duration_cast<std::chrono::nanoseconds>(finish_time - start_time).count();
        if (tempTime == 0) tempTime = 10;
        std::cerr << "(" << setiosflags(ios::fixed) << setw(9) << setprecision(9) << tempTime * 1e-9 << " s)" << std::endl;
    }
    }
    catch (std::out_of_range) {
        throw std::runtime_error("SYNTAX ERROR: You have an error in your SQL syntax (Drop table)");
    }
}

//传给API 表名，属性名，索引名
void Parser::Create_index(vector<string> args){
    //create index index_name on tablename (attri_name)
    // 0      1      2         3    4       5   6     7
    try{
    string indexname=args.at(2);
    if(args.at(3)!="on") {throw std::runtime_error("SYNTAX ERROR: You have an error in your SQL syntax (Create index)");}
    else{
        int i=4;
        string tablename=args.at(i++);
        if(args.at(i)=="("&&args.at(i+2)==")"){
            string at_name=args.at(i+1);
            if(args.size()==i+3) {
                /*传给API表名，索引名，属性名，
                //测试该表的这个属性是否唯一*/
                auto start_time = std::chrono::high_resolution_clock::now();
                API_create_index(tablename,indexname,at_name);
                auto finish_time = std::chrono::high_resolution_clock::now();
                int tempTime = std::chrono::duration_cast<std::chrono::nanoseconds>(finish_time - start_time).count();
                if (tempTime == 0) tempTime = 10;
                std::cerr << "(" << setiosflags(ios::fixed) << setw(9) << setprecision(9) << tempTime * 1e-9 << " s)" << std::endl;
                cout<<"table: "<<tablename<<" index: "<<indexname<<" attri_name: "<<at_name<<endl;
            }
            else { throw std::runtime_error("SYNTAX ERROR: You have an error in your SQL syntax (Create index)");}
        }
        else { throw std::runtime_error("SYNTAX ERROR: You have an error in your SQL syntax (Create index)");}
    }
    }
    catch (std::out_of_range) {
        throw std::runtime_error("SYNTAX ERROR: You have an error in your SQL syntax (Create index)");
    }

}

//传给API 索引名
void Parser::Drop_index(vector<string> args){
    //drop index name
    //   0      1   2
    try{
    string indexname;
    indexname=args.at(2);
    if(args.size()>3) { throw std::runtime_error("SYNTAX ERROR: You can only drop one index once");}
    else {
        cout<<"API drop index "<<indexname<<endl;
        /* 调用API，只有一个表名作参数
        测试有没有这个索引
        IndexManager_exist(indexname);
        */
        auto start_time = std::chrono::high_resolution_clock::now();
        API_drop_index(indexname);
        auto finish_time = std::chrono::high_resolution_clock::now();
        int tempTime = std::chrono::duration_cast<std::chrono::nanoseconds>(finish_time - start_time).count();
        if (tempTime == 0) tempTime = 10;
        std::cerr << "(" << setiosflags(ios::fixed) << setw(9) << setprecision(9) << tempTime * 1e-9 << " s)" << std::endl;
    }
    }
    catch (std::out_of_range) {
        throw std::runtime_error("SYNTAX ERROR: You have an error in your SQL syntax (Drop index)");
    }
}

//传给API 表名，条件vector<condition>
void Parser::Select(vector<string> args){
    //select * from tablename where a<>'sda' and b<='ads' and c>=12 and d=12.4;
    // 0     1   2    3        4    
    try{
    string tablename=args.at(3);
    int i=4;
    vector<condition> conditions; 
    //没有条件
    if(args.size()==4){
        auto start_time = std::chrono::high_resolution_clock::now();
        //API_select(tablename,conditions);
        auto finish_time = std::chrono::high_resolution_clock::now();
        int tempTime = std::chrono::duration_cast<std::chrono::nanoseconds>(finish_time - start_time).count();
        if (tempTime == 0) tempTime = 10;
        std::cerr << "(" << setiosflags(ios::fixed) << setw(9) << setprecision(9) << tempTime * 1e-9 << " s)" << std::endl;
        return;
    }
    if(args.at(i)!="where") {throw std::runtime_error("SYNTAX ERROR: You have an error in your SQL syntax (Select)");}
    i++;
    while(1){
        condition con;
        //name
        con.name=args.at(i++);
        //op
        if(args.at(i)=="=")  con.op=0;
        else if(args.at(i)=="<"&&args.at(i+1)==">") { con.op=1; i++;}
        else if(args.at(i)=="<"&&args.at(i+1)=="=") { con.op=5; i++;}
        else if(args.at(i)=="<")    con.op=4;
        else if(args.at(i)==">"&&args.at(i+1)=="=") { con.op=3; i++;}
        else if(args.at(i)==">")                    con.op=2;
        else { cout<<"syntax error!"<<endl; return;}
        i++;
        //sqlvalue
        string value=args.at(i);
        if(value.at(0)=='\''&&value.at(value.length()-1)=='\''){
            con.val.type.type=AType::String;
            con.val.type.attri_name=con.name;
            con.val.str=value.substr(1,value.length()-2);
        }
        else if(value.find('.')!=string::npos){  //fraction
            con.val.f=stof(value);
            con.val.type.type=AType::Float;
            con.val.type.attri_name=con.name;
        }
        else{      //int
            con.val.i=stoi(value);
            con.val.type.type=AType::Integer;
            con.val.type.attri_name=con.name;
        }
        conditions.push_back(con);
        if(i==(int)args.size()-1) break;
        else if(args.at(i+1)=="and") i+=2;
        else break;
    }
    //debug
    auto itr=conditions.begin();
    for(;itr!=conditions.end();itr++){
        cout<<itr->val.type.attri_name<<" "<<itr->op<<" ";
        cout<<itr->val.toStr();
        cout<<endl;
    }
    //检查表，检查参数模块没有
    //这里有点问题，应该还要对照表的列属性对比检查一遍
    //检查可以放到API里面
    auto start_time = std::chrono::high_resolution_clock::now();
    API_select(tablename,conditions);
    auto finish_time = std::chrono::high_resolution_clock::now();
    int tempTime = std::chrono::duration_cast<std::chrono::nanoseconds>(finish_time - start_time).count();
    if (tempTime == 0) tempTime = 10;
    std::cerr << "(" << setiosflags(ios::fixed) << setw(9) << setprecision(9) << tempTime * 1e-9 << " s)" << std::endl;
    }
    catch (std::out_of_range) {
        throw std::runtime_error("SYNTAX ERROR: You have an error in your SQL syntax (Select)");
    }
}

void Parser::Selectpart(vector<string> args){
    //select a1 a2 from tablename where a<>'sda' and b<='ads' and c>=12 and d=12.4;
    // 0     1   2    3        4    5
    try{
    int i=1;
    vector<string> s_attris;
    while(args.at(i)!="from"){
        string selecta;
        selecta=args.at(i++);
        s_attris.push_back(selecta);
    }
    string tablename=args.at(++i);
    vector<condition> conditions; 
    
    if(args.size()==i+1){
        auto start_time = std::chrono::high_resolution_clock::now();
        API_selectpart(s_attris,tablename,conditions);
        auto finish_time = std::chrono::high_resolution_clock::now();
        int tempTime = std::chrono::duration_cast<std::chrono::nanoseconds>(finish_time - start_time).count();
        if (tempTime == 0) tempTime = 10;
        std::cerr << "(" << setiosflags(ios::fixed) << setw(9) << setprecision(9) << tempTime * 1e-9 << " s)" << std::endl;
        return;
    }
    if(args.at(++i)!="where") {throw std::runtime_error("SYNTAX ERROR: You have an error in your SQL syntax (Select)");}
    i++;
    while(1){
        condition con;
        //name
        con.name=args.at(i++);
        //op
        if(args.at(i)=="=")  con.op=0;
        else if(args.at(i)=="<"&&args.at(i+1)==">") { con.op=1; i++;}
        else if(args.at(i)=="<"&&args.at(i+1)=="=") { con.op=5; i++;}
        else if(args.at(i)=="<")    con.op=4;
        else if(args.at(i)==">"&&args.at(i+1)=="=") { con.op=3; i++;}
        else if(args.at(i)==">")                    con.op=2;
        else { cout<<"syntax error!"<<endl; return;}
        i++;
        //sqlvalue
        string value=args.at(i);
        if(value.at(0)=='\''&&value.at(value.length()-1)=='\''){
            con.val.type.type=AType::String;
            con.val.type.attri_name=con.name;
            con.val.str=value.substr(1,value.length()-2);
        }
        else if(value.find('.')!=string::npos){  //fraction
            con.val.f=stof(value);
            con.val.type.type=AType::Float;
            con.val.type.attri_name=con.name;
        }
        else{      //int
            con.val.i=stoi(value);
            con.val.type.type=AType::Integer;
            con.val.type.attri_name=con.name;
        }
        conditions.push_back(con);
        if(i==(int)args.size()-1) break;
        else if(args.at(i+1)=="and") i+=2;
        else break;
    }
    //debug
    auto tr=s_attris.begin();
    cout<<"select: ";
    for(;tr!=s_attris.end();tr++){
        cout<<*tr<<" ";
    }
    cout<<endl;
    auto itr=conditions.begin();
    for(;itr!=conditions.end();itr++){
        cout<<itr->val.type.attri_name<<" "<<itr->op<<" ";
        cout<<itr->val.toStr();
        cout<<endl;
    }
    //检查表，检查参数模块没有
    //这里有点问题，应该还要对照表的列属性对比检查一遍
    //检查可以放到API里面
    auto start_time = std::chrono::high_resolution_clock::now();
    API_selectpart(s_attris, tablename,conditions);
    auto finish_time = std::chrono::high_resolution_clock::now();
    int tempTime = std::chrono::duration_cast<std::chrono::nanoseconds>(finish_time - start_time).count();
    if (tempTime == 0) tempTime = 10;
    std::cerr << "(" << setiosflags(ios::fixed) << setw(9) << setprecision(9) << tempTime * 1e-9 << " s)" << std::endl;
    }
    catch (std::out_of_range) {
        throw std::runtime_error("SYNTAX ERROR: You have an error in your SQL syntax (Select)");
    }
}

void Parser::Insert(vector<string> args){
    //insert into tablename values('asdsda',12,12.3);
    //0       1     2        3     4  5    6   7  8  9   10
    try{  
    if(args.at(1)!="into") { throw std::runtime_error("SYNTAX ERROR: You have an error in your SQL syntax (Insert)"); }
    string tablename=args.at(2);
    if(args.at(3)!="values"||args.at(4)!="(") {throw std::runtime_error("SYNTAX ERROR: You have an error in your SQL syntax (Insert)");} 
    vector<sqlvalue>  value_list;
    //本来是根据表的属性顺序来判断，这里先按字面译码，进API再判断
    int i=5;
    while(1){
        sqlvalue v;
        string p=args.at(i);
        if(p.at(0)=='\''&&p.at(p.length()-1)=='\''){
            v.type.type=AType::String;   
            v.str=p.substr(1,p.length()-2);
        }
        else if(p.find('.')!=string::npos){
            v.type.type=AType::Float;
            v.f=stof(p);
        }
        else{
            v.type.type=AType::Integer;
            v.i=stoi(p);
        }
        value_list.push_back(v);
        if(args.at(i+1)==",") i+=2;
        else i++;
        if(args.at(i)==")") break;
    }
    //debug
    auto itr=value_list.begin();
    for(;itr!=value_list.end();itr++){
        cout<<itr->toStr();
        cout<<endl;
    }
    
    auto start_time = std::chrono::high_resolution_clock::now();
    API_insert(tablename,value_list);
    auto finish_time = std::chrono::high_resolution_clock::now();
    int tempTime = std::chrono::duration_cast<std::chrono::nanoseconds>(finish_time - start_time).count();
    if (tempTime == 0) tempTime = 10;
    std::cerr << "(" << setiosflags(ios::fixed) << setw(9) << setprecision(9) << tempTime * 1e-9 << " s)" << std::endl;
    }
    catch (std::out_of_range) {
        throw std::runtime_error("SYNTAX ERROR: You have an error in your SQL syntax (Insert)");
    }
}

void Parser::Delete(vector<string> args){
    //delete from tablename where a<>'sda' and b<='ads' and c>=12 and d=12.4;
    //0      1     2        3      4
    try{
    if(args.at(1)!="from"){ throw std::runtime_error("SYNTAX ERROR: You have an error in your SQL syntax (delete)");}
    string tablename=args.at(2);
    if(args.at(3)!="where"){ throw std::runtime_error("SYNTAX ERROR: You have an error in your SQL syntax (delete)");}
    vector<condition> conditions;
    int i=4;
    while(1){
        condition con;
        //name
        con.name=args.at(i++);
        //op
        if(args.at(i)=="=")  con.op=0;
        else if(args.at(i)=="<"&&args.at(i+1)==">") { con.op=1; i++;}
        else if(args.at(i)=="<"&&args.at(i+1)=="=") { con.op=5; i++;}
        else if(args.at(i)=="<")    con.op=4;
        else if(args.at(i)==">"&&args.at(i+1)=="=") { con.op=3; i++;}
        else if(args.at(i)==">")                    con.op=2;
        else { throw std::runtime_error("SYNTAX ERROR: You have an error in your SQL syntax (delete)");}
        i++;
        //sqlvalue
        string value=args.at(i);
        if(value.at(0)=='\''&&value.at(value.length()-1)=='\''){
            con.val.type.type=AType::String;
            con.val.type.attri_name=con.name;
            con.val.str=value.substr(1,value.length()-2);
        }
        else if(value.find('.')!=string::npos){  //fraction
            con.val.f=stof(value);
            con.val.type.type=AType::Float;
            con.val.type.attri_name=con.name;
        }
        else{      //int
            con.val.i=stoi(value);
            con.val.type.type=AType::Integer;
            con.val.type.attri_name=con.name;
        }
        conditions.push_back(con);
        if(i==(int)args.size()-1) break;
        else if(args.at(i+1)=="and") i+=2;
        else break;
    }
    //debug
    auto itr=conditions.begin();
    for(;itr!=conditions.end();itr++){
        cout<<itr->val.type.attri_name<<" "<<itr->op<<" ";
        cout<<itr->val.toStr();
        cout<<endl;
    }
   
    //call API and timer
    auto start_time = std::chrono::high_resolution_clock::now();
    API_delete(tablename,conditions);   
    auto finish_time = std::chrono::high_resolution_clock::now();
    int tempTime = std::chrono::duration_cast<std::chrono::nanoseconds>(finish_time - start_time).count();
    if (tempTime == 0) tempTime = 10;
    std::cerr << "(" << setiosflags(ios::fixed) << setw(9) << setprecision(9) << tempTime * 1e-9 << " s)" << std::endl;
    }
    catch (std::out_of_range) {
        throw std::runtime_error("SYNTAX ERROR: You have an error in your SQL syntax (delete)");
    }
}

void Parser::Execfile(vector<string> args){
    
    if(args.size()>2) { cout<<"syntax error!"<<endl; return;}
    Parser parser;
    string ins;
    string filename=args.at(1);
    ifstream infile(filename);
    while (getline(infile, ins)) {
       try{ parser.input(ins); }
       catch (std::runtime_error &error) {
            cout << "[Error] " << error.what() << endl;
            parser.flushBuffer();
        }
    } 
}
