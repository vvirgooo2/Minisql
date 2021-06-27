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
            getchar();
            exit(0);
        }
        try{ 
            State = parser.input(ins);  //false stands for input continue
        }
        catch (std::runtime_error &error) {
            cout << "[Error] " << error.what() << endl;
            parser.flushBuffer();
            State = true;
        }
    }
}
