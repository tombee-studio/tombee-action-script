#pragma once

#include <cstdlib>
#include "lexer.hpp"
#include "ast.hpp"

class ParseError: public runtime_error {
public:
    ParseError(const char *message): runtime_error(message) {}
};

class Parser {
    Parser() {}
public:
    static Parser* getInstance();
    Program *parse(vector<Token>&);
    Script *parse_script(vector<Token>&, int&);
    Event *parse_event(vector<Token>&, int&);
    Statement *parse_statement(vector<Token>&, int&);
    Statement *parse_block(vector<Token>&, int&);
    Statement *parse_loop(vector<Token>&, int&);
    Statement *parse_ifst(vector<Token>&, int&);
    Expression *parse_expression(vector<Token>&, int&);
    Expression *parse_declare(vector<Token>&, int&);
    Expression *parse_assign(vector<Token>&, int&);
    Expression *parse_equality(vector<Token>&, int&);
    Expression *parse_compare(vector<Token>&, int&);
    Expression *parse_add(vector<Token>&, int&);
    Expression *parse_mul(vector<Token>&, int&);
    Expression *parse_unary(vector<Token>&, int&);
    Expression *parse_term(vector<Token>&, int&);
    Expression *parse_call(vector<Token>&, int&);
private:
    Token* consume(vector<Token>&, int&, int);
};
