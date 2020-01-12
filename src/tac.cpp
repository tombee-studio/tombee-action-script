#include "../include/tac.hpp"

TACOperand 
TACOperand::make(int mnemonic, int type, Primitive value) {
    TACOperand operand;
    operand.mnemonic = mnemonic;
    operand.type = type;
    operand.value = value;
    operand.argsNum = 0;
    return operand;
}

TACOperand 
TACOperand::make(int mnemonic, int type, Primitive value, int argsNum) {
    TACOperand operand;
    operand.mnemonic = mnemonic;
    operand.type = type;
    operand.value = value;
    operand.argsNum = argsNum;
    return operand;
}

void 
TACOperand::print() {
    switch(mnemonic) {
    case EXIT:
        cout << "EXIT";
        break;
    case NEXT:
        cout << "NEXT";
        break;
    case PUSH:
        cout << "PUSH";
        break;
    case POP:
        cout << "POP";
        break;
    case DECL:
        cout << "DECL";
        break;
    case ASSIGN:
        cout << "ASSIGN";
        break;
    case JE:
        cout << "JE";
        break;
    case JNE:
        cout << "JNE";
        break;
    case JMP:
        cout << "JMP";
        break;
    case EQ:
        cout << "EQ";
        break;
    case NE:
        cout << "NE";
        break;
    case LT:
        cout << "LT";
        break;
    case LE:
        cout << "LE";
        break;
    case GT:
        cout << "GT";
        break;
    case GE:
        cout << "GE";
        break;
    case ADD:
        cout << "ADD";
        break;
    case SUB:
        cout << "SUB";
        break;
    case MUL:
        cout << "MUL";
        break;
    case DIV:
        cout << "DIV";
        break;
    case REV:
        cout << "REV";
        break;
    case LOAD:
        cout << "LOAD";
        break;
    case CALL:
        cout << "CALL";
        break;
    case STAGED:
        cout << "STAGED";
        break;
    case UNSTAGED:
        cout << "UNSTAGED";
        break;
    case LOOPSTART:
        cout << "LOOPSTART";
        break;
    case LOOPEND:
        cout << "LOOPEND";
        break;
    }
    switch(type) {
    case Primitive::INT:
        cout << " INT";
        break;
    case Primitive::FLOAT:
        cout << " FLOAT";
        break;
    case Primitive::ID:
        cout << " ID";
        break;
    case Primitive::NONE:
        break;
    }
    cout << " " << (string)value;
    if(argsNum > 0) {
        cout << " " << argsNum;
    }
    cout << endl;
}
