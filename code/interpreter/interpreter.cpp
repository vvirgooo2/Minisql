#include"interpreter.h"
#include<string>
#include<iostream>
#include<stdexcept>
using namespace std;
void interpreter::Loop(){
    string ins;
    static bool State = true;  //true
    while(1){
        if(State) cout<<"MiniSql> ";
        else      cout<<"       >";
        getline(cin,ins);
        if(ins=="quit"){
            cout << "Bye!" << endl;
            exit(0);
        }
        State = parser.input(ins);  //false代表还没输入完
    }
}