#ifndef _SQLDATATYPE_H_
#define _SQLDATATYPE_H_

#include<vector>
#include<iostream>
#include<utility>
#include<string>
using namespace std;
/*这个头文件主要定义一些数据结构来存储表，条件，属性，sql值等信息*/

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
        }
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
    Row fetchRow(const std::vector<std::string> &attrTable, const std::vector<std::string> &attrFetch) const {
            Row row;
            bool attrFound;
            row.col.reserve(attrFetch.size());
            for (auto fetch : attrFetch) {
                attrFound = false;
                for (int i = 0; i < attrTable.size(); i++) {
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
#endif