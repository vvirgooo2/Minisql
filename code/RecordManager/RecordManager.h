#ifndef _RECORDMANAGER_H_
#define _RECORDMANAGER_H_
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include "../SqlDataType.h"
using namespace std;

class RecordManager{
private:
    void print(const Result &res) const;   //打印结果
    //检查元组是否符合条件
    bool validCheck(const vector<condition> conditions, const Tuple tu, const vector<string> attris);
    //从内存中读取元组
    
public:
    //创建表文件
    bool createTable(const string tablename);
    //删除表文件
    bool dropTable(const string tablename);
    //查找记录,返回条数,无索引
    int  selectRecord(const Table &table, const vector<string> &attr, const vector<condition> conditions);
    //查找记录，返回条数，有索引
    //
    //插入记录
    bool insertRecord(const Table &table, const Tuple &record);
    //删除记录
    bool deleteRecord(const Table &table, const vector<condition> conditions);
};



#endif