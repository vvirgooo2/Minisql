#include "API.h"
#include"../SqlDataType.h"
#include"../interpreter/interpreter.h"
#include"../RecordManager/RecordManager.h"
#include<vector>
#include<string>
using namespace std;
//API层建表
//功能：
//先判断有无表名冲突
//判断有无主键
//判断属性是否符合条件
//传给Catalog建表，传给Record建文件，传给Index建主键索引
void API_create_table(string tablename,vector<attri_type>attris){
    cout<<"API Create"<<endl;
}

//删除表
//判断存在
//Catalog删除信息，Record删除表文件，Index删除有关索引
void API_drop_table(string tablename){
    cout<<"API Drop"<<endl;
}

//创建索引：
//判断表，属性
//传给Index建表，Catalog更新信息
void API_create_index(string tablename,string indexname,string at_name){
    cout<<"API ci"<<endl;
}

//删除索引
//传给index删除索引，Catalog更新信息
void API_drop_index(string indexname){
    
}

//选择（全选）
//判断表名，判断条件是否合理
//传给record
void API_select(string tablename, vector<condition> conditions){
    cout<<"API Select"<<endl;
}

//选择（部分）
//判断属性名，条件的合理性
//注意属性名参数只有name成员被赋值了
void API_selectpart(vector<attri_type> attris, string tablename, vector<condition> conditions){
    cout<<"API Selectpart"<<endl;
}


//插入
//判断表名
//预处理值表（判断和表是否对应）
//给record插入
void API_insert(string tablename,vector<sqlvalue> value_list){

}

//删除
//判断表名
//预处理条件
//给record删除
void API_delete(string tablename,vector<condition> conditions){

}