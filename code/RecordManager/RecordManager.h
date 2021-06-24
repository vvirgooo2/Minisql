#ifndef _RECORDMANAGER_H_
#define _RECORDMANAGER_H_
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include "../sqlDataType.h"

using namespace std;

class RecordManager{
public:
    //打印结果
    void print(const Result &res) const;   
    //检查元组是否符合条件
    bool validCheck(const vector<condition> conditions, const Tuple tu);
    //给定块和offset，把一个tuple读出来
    void readTuple(const char *blockBuffer,int offset, const vector<attri_type> &attris, Tuple&tu);
    
public:
    //创建表文件
    bool createTable(const string tablename);
    //删除表文件
    bool dropTable(const string tablename);
    //查找记录,返回条数,无索引
    int  selectRecord(const Table &table, const vector<string> &attr, const vector<condition> conditions,bool output=false);
    //查找记录，返回条数，有索引
    int  selectRecord_index(const Table &table, const vector<string> &attr, const vector<condition> conditions,const condition indexcon,bool output=false);
    //插入记录
    bool insertRecord(const Table &table, const Tuple &record);
    //删除记录
    bool deleteRecord(const Table &table, const vector<condition> conditions);
    //创建索引
    bool CreateIndex(const Table &table, const attri_type indexattr);
};



#endif