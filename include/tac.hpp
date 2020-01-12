#pragma once

#include "primitive.hpp"

struct TACOperand {
    enum {
        EXIT = -1,
        NEXT,
        PUSH,
        POP,
        DECL,
        ASSIGN,
        EQ,
        NE,
        LT,
        LE,
        GT,
        GE,
        ADD,
        SUB,
        MUL,
        DIV,
        REV,
        LOAD,
        CALL,
        JE,
        JNE,
        JMP,
        STAGED,
        UNSTAGED,
        LOOPSTART,
        LOOPEND,
    };
    int mnemonic;
    int type;
    Primitive value;
    int argsNum;
    static TACOperand make(int, int, Primitive);
    static TACOperand make(int, int, Primitive, int);

    void print();
};
