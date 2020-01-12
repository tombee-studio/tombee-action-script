#pragma once

#include <cassert>
#ifndef ASSERT
#define ASSERT(x) assert(x);
#endif

#include <iostream>
using namespace std;

struct Token {
    enum {
        TK_EOF = -1,
        TK_INT,
        TK_FLOAT,
        TK_STRING,
        TK_ID,
        KW_IF,
        KW_ELSE,
        KW_LOOP,
        KW_SHOT,
        KW_FUNC,
        KW_INT,
        KW_FLOAT,
        KW_REF,
        KW_VAR,
        KW_CASE,
        KW_INIT,
        KW_UPDATE,
        KW_INTERRUPT,
        KW_END,
        OP_EQ,
        OP_NE,
        OP_AND,
        OP_OR,
        OP_LE,
        OP_GE,
        OP_AA,
        OP_SA,
        OP_INC,
        OP_DEC,
        COMMENT,
    };

    int type;

    union {
        int ival;
        double fval;
    };

    string id;

    static Token make_int(int);
    static Token make_float(double);
    static Token make_id(string);
    static Token make_token(int);

    operator string();
};
