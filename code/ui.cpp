#include<cstdio>
#include<string.h>
#include<stdlib.h>
#include"interpreter\interpreter.h"

void help_infor();
int main(){
    printf("Welcome to our minisql!\n Use help to get the information\n");
    while(1){
        char ins[50];
        printf("> ");
        scanf("%s",ins);
        if(!strcmp(ins, "help")) help_infor();
        else if(!strcmp(ins, "exit")){
            printf("Bye.\n");
            break;
        }
        else 
            if(!interpreter(ins)) printf("Invalid instruction.\n");
    }
    getchar();
    getchar();
}

void help_infor(){
    printf("No help, help yourself!\n");
}