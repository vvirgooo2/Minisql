cd D:\coding\c++\minisql\code
g++ ui.cpp ./interpreter/interpreter.cpp ./interpreter/Parser.cpp ./SqlDataType.h -std=c++11 -o ../minisql.exe
start ../minisql.exe