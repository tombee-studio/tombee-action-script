#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "token.hpp"

namespace tas {
using namespace std;

class LexerError: public runtime_error {
public:
    LexerError(const string& msg): runtime_error(msg) {}
};

class Lexer {
    vector<Token> tokens;
    Lexer() {}
public:
    static Lexer* getInstance();
    vector<Token> lex(string);
    vector<Token> lex_string(string);
private:
    void tokenize(string&, int&);
    bool tokenizeOperator(string&, int&, char);
    bool tokenizeOperator(string&, int&, int, string);
    bool tokenizeKeyword(string&, int&, int, string);
    bool tokenizeNum(string&, int&);
    bool tokenizeId(string&, int&);
    bool tokenizeStr(string&, int&);
    bool skip(string&, int&);
    bool comment(string&, int&);
};

} // namespace tas
