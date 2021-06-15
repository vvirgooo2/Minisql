#include<cstdio>
#include<string.h>
#include"interpreter\interpreter.h"
#include"SqlDataType.h"
int main(){
    printf("Welcome to our minisql!\nUse help to get the information\n");
    interpreter inter;
    inter.Loop();
}
