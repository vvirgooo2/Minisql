#ifndef _SQLDATATYPE_H_
#define _SQLDATATYPE_H_

#include<vector>
#include<iostream>
#include<utility>
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
};

//用来存储条件 例如 where ID = '123'
struct condition{
    string name;  //属性名
    int op;       //0= 1<> 2> 3>= 4< 5<= 
    sqlvalue val; 
};

//存储表的数据结构
struct table{
    vector<string> attri_names;        //各列的名字
    vector<attri_type> attri_types;    //各列属性
    vector<pair<string,string>> index; //属性名-索引名
    int attri_count;                   //属性个数
    int row_num;                       //记录条数
};

#endif