#ifndef _PARSER_H_
#define _PARSER_H_

#include<deque>

class Parser{
public:
    Parser(){}
    bool input(std::string ins);
    void flushBuffer();
private:
    std::deque<std::string> buffer;

};

#endif 