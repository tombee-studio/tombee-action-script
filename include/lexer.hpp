#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include "token.hpp"
using namespace std;

class Lexer {
    vector<Token> tokens;

    Lexer(){}
public:
    static Lexer* getInstance();
    vector<Token> lex(string);
private:
    void tokenize(string&, int&);
    bool tokenizeKeyword(string&, int&, int, string);
    bool tokenizeOperator(string&, int&, char);
    bool tokenizeOperator(string&, int&, int, string);
    bool tokenizeId(string&, int&);
    bool tokenizeNum(string&, int&);
    bool tokenizeStr(string&, int&);
    bool comment(string&, int&);
    bool skip(string&, int&);
};
