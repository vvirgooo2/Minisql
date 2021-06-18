#include"RecordManager.h"

void RecordManager::print(const Result &res) const{ //打印结果

}  

//检查元组是否符合条件
bool RecordManager::validCheck(const vector<condition> conditions, const Tuple tu, const vector<string> attris)
{
    
}
    
//从内存中读取元组

//创建表文件
bool RecordManager::createTable(const string tablename)
{

}

//删除表文件
bool RecordManager::dropTable(const string tablename)
{

}

//查找记录,返回条数,无索引
int  RecordManager::selectRecord(const Table &table, const vector<string> &attr, const vector<condition> conditions)
{

}

//查找记录，返回条数，有索引
//
//插入记录
bool RecordManager::insertRecord(const Table &table, const Tuple &record)
{

}
    
//删除记录
bool RecordManager::deleteRecord(const Table &table, const vector<condition> conditions)
{

}