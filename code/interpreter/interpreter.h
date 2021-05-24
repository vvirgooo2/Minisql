#ifndef _INTERPRETER_H_
#define _INTERPRETER_H_
#include<cstdio>
#include<string>
#include"Parser.h"

//interpreter类
//loop函数中等待输入
//输入完毕后给Parser类进行译码
class interpreter{
public:
    interpreter(){}
    void Loop();      //不断循环
private:
    Parser parser;    
};
#endif