#include "API.h"
#include"../SqlDataType.h"
#include"../interpreter/interpreter.h"
#include"../RecordManager/RecordManager.h"
#include<vector>
#include<string>
using namespace std;
RecordManager *rm;
Table get_test_table(){
 //属性
    vector<attri_type> attris;
    attri_type attr;
    attr.type=AType::Integer;
    attr.attri_name="id";
    attris.push_back(attr);

    attr.type=AType::String;
    attr.attri_name="name";
    attr.char_sz=12;
    attris.push_back(attr);

    attr.type=AType::Float;
    attr.attri_name="score";
    attris.push_back(attr);

    //Fetch
    vector<string> Fetch;
    Fetch.push_back("id");
    Fetch.push_back("name");
    Fetch.push_back("score");

    Table table;
    table.tablename="student2";
    table.attri_names=Fetch;
    table.attri_types=attris;
    table.attri_count=3;
    return table;

}


//API层建表
//功能：
//先判断有无表名冲突
//判断有无主键
//判断属性是否符合条件
//传给Catalog建表，传给Record建文件，传给Index建主键索引
void API_create_table(string tablename,vector<attri_type>attris){

}

//删除表
//判断存在
//Catalog删除信息，Record删除表文件，Index删除有关索引
void API_drop_table(string tablename){

}

//创建索引：
//判断表，属性
//传给Index建表，Catalog更新信息
void API_create_index(string tablename,string indexname,string at_name){

}

//删除索引
//传给index删除索引，Catalog更新信息
void API_drop_index(string indexname){
    
}

//选择（全选）
//判断表名，判断条件是否合理
//传给record
void API_select(string tablename, vector<condition> conditions){
    Table table=get_test_table();
    rm->selectRecord(table,table.attri_names,conditions);

}

//选择（部分）
//判断属性名，条件的合理性
//注意属性名参数只有name成员被赋值了
void API_selectpart(vector<string> attris, string tablename, vector<condition> conditions){
    Table table=get_test_table();
    rm->selectRecord(table,attris,conditions);
}


//插入
//判断表名
//预处理值表（判断和表是否对应）,一定要把string的长度填充上！！！！！！
//调用record的select或索引判断主键或unique键的冲突
//无问题给record插入
void API_insert(string tablename,vector<sqlvalue> value_list){
    Tuple t;
    t.element=value_list;
    t.element[1].type.char_sz=12;
    //测试块
    Table table=get_test_table();
    rm->insertRecord(table,t);
}

//删除
//判断表名
//预处理条件
//给record删除
void API_delete(string tablename,vector<condition> conditions){

}