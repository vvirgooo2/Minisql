#include"interpreter.h"
#include<string>
#include<iostream>
#include<stdexcept>
void interpreter::Loop(){
    std::string ins;
    static bool State = true;  //true
    while(1){
        if(State) std::cout<<"SQL> ";
        else std::cout<<"   >";
        std::getline(std::cin,ins);
        try{
            State = parser.input(ins);  //false代表还没输入完
        } catch(std::runtime_error &error){
            std::cout << "[Error] " << error.what() << std::endl;
            parser.flushBuffer();                
            State = true;
        }
    }
}