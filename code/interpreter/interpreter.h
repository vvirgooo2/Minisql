#ifndef _INTERPRETER_H_
#define _INTERPRETER_H_
#endif

#include<cstdio>
#include<string>

//interpreter函数负责翻译命令，并向下传递给api层，
//译码成功返回true，译码失败返回false
//译码成功向下传递仍可能出错
bool interpreter(char * ins);
