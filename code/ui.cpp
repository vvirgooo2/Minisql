#include<cstdio>
#include<string.h>
#include"interpreter/interpreter.h"
#include"SqlDataType.h"
int main(){
    printf("Welcome to our minisql!\n");
    interpreter inter;
    inter.Loop();
}