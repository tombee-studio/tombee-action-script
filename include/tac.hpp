#pragma once

#include "primitive.hpp"

namespace tas {

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
        MOD,
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
        GLOBAL_DECL,
        DELAY,
        DISPATCH,
        YIELD,
    };
    int mnemonic;
    int type;
    Primitive value;
    int argsNum;
    static TACOperand make(int, int, Primitive);
    static TACOperand make(int, int, Primitive, int);

    void print();
};

} // namespace tas
