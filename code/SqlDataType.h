#ifndef _SQLDATATYPE_H_
#define _SQLDATATYPE_H_

#include <bits/stdc++.h>
#include<vector>
#include<iostream>
#include<utility>
#include<string>
using namespace std;
/*这个头文件主要定义一些数据结构来存储表，条件，属性，sql值等信息*/
#define MAX_TableName 128
#define MAX_BlockNumber 10000
#define BLOCK_SIZE 4096
#define INSERT_OP 0
#define SELECT_OP 1
#define DELETE_OP 2

//属性枚举类型
enum class AType{
    Integer,
    Float,
    String
};

//单个列的属性
struct attri_type{
    bool unique=false;
    bool primary=false;
    string attri_name;

    AType type;
    int char_sz=0; //0-255

    int getsize(){
        switch(type){
            case AType::Integer: return sizeof(int); break;
            case AType::Float: return sizeof(float); break;
            case AType::String: return char_sz; break;
        }
        return 0;
    }
};

//单个sql值
struct sqlvalue{
    attri_type type;  //属性,只用name和type
    int i;
    float f;
    string str;
    //可能会定义一些有用的函数

    string toStr() const{
        switch(type.type){
            case AType::Integer:
                return to_string(i);
            case AType::Float:
                return to_string(f);
            case AType::String:
                return str;
            default:
                return "error";
        }
        return "error";
    }
    void reset(){
        str.clear();
        i = 0;
        f = 0;
    }
    //操作符重载，可利用操作符比较sqlvalue
    bool operator<(const sqlvalue &e) const{
        switch(type.type){
            case AType::Integer: return i<e.i;
            case AType::Float:   return f<e.f;
            case AType::String:  return str<e.str;
            default: throw std::runtime_error("Undefined Type!");
        }
    }
    bool operator==(const sqlvalue &e) const{
        switch(type.type){
            case AType::Integer: return i==e.i;
            case AType::Float:   return f==e.f;
            case AType::String:  return str==e.str;
            default: throw std::runtime_error("Undefined Type!");
        }
    }
    bool operator!=(const sqlvalue &e) const{
        return !operator==(e);
    }
    bool operator>(const sqlvalue &e) const{
        return !operator<(e)&&operator!=(e);
    }
    bool operator<=(const sqlvalue &e) const{
        return operator<(e) || operator==(e);
    }
    bool operator>=(const sqlvalue &e) const{
        return !operator<(e);
    }

};

//用来存储条件 例如 where ID = '123'
struct condition{
    string name;  //属性名
    int op;       //0= 1<> 2> 3>= 4< 5<=
    sqlvalue val;
};

//存储表的数据结构
struct Table{
    string tablename;
    vector<string> attri_names;        //各列的名字
    vector<attri_type> attri_types;    //各列属性
    vector<pair<string,string>> index; //属性名-索引名
    int attri_count;                   //属性个数
    int row_num;                       //记录条数
};

//存放一行已经转化为string的值
struct Row{
    vector<string> col;
};
//存放一次结果的result
struct Result{
    vector<Row> row;
};
//一行元组
struct Tuple{
    vector<sqlvalue> element; //存sqlvalue型的一行的值
    //选出所需要的列
    Row fetchRow(const std::vector<std::string> &attrTable, const std::vector<std::string> &attrFetch) const {
            Row row;
            bool attrFound;
            row.col.reserve(attrFetch.size());
            for (auto fetch : attrFetch) {
                attrFound = false;
                for (size_t i = 0; i < attrTable.size(); i++) {
                    if (fetch == attrTable[i]) {
                        row.col.push_back(element[i].toStr());
                        attrFound = true;
                        break;
                    }
                }
                if (!attrFound) {
                    std::cerr << "Undefined attr in row fetching!!" << std::endl;
                }
            }
            return row;
        }
};
/* class Position and IndexInfo are the 2 data structures that exchanging information between Record Manager*/
//Position contain the
class Position
{
    public:
        int blockID;
        int offset;//don't count the valid bit into it
        void clear()
        {
            blockID = 0;
            offset = 0;
        }
};
//data structure that interact with index manager
template <typename T>
class IndexInfo
{
    public:
        int capacity; //the number of keys preview
        int size; //the number of keys
        int key_maxsize; // the size of the key
        AType type;
        T* keys;
        Position *p;
        string tablename, attr_name;
        IndexInfo(const string &tablename, int capacity, const string& attr_name, AType type, const int key_maxsize)
        :capacity(capacity), tablename(tablename), attr_name(attr_name), type(type), size(0), key_maxsize(key_maxsize)
        {
            keys = new T[capacity];
            p = new Position[capacity];
        }
        ~IndexInfo(){
            delete []keys;
            delete []p;
        };
        void AddKey(const T key, const int blockid, const int offset)
        {
            keys[size] = key;
            p[size].blockID = blockid;
            p[size++].offset = offset;
        }
};

#endif
