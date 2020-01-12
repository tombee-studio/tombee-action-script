#include "../include/token.hpp"

Token 
Token::make_int(int ival) {
    Token token;
    token.ival = ival;
    token.type = TK_INT;
    return token;
}

Token
Token::make_float(double fval) {
    Token token;
    token.fval = fval;
    token.type = TK_FLOAT;
    return token;
}

Token
Token::make_id(string str) {
    Token token;
    token.id = str.c_str();
    token.type = TK_ID;
    return token;
}

Token
Token::make_token(int type) {
    Token token;
    token.type = type;
    return token;
}

Token::operator string() {
    switch(type) {
    case TK_INT:
        return "[(int)" + to_string(ival) + "]";
    case TK_FLOAT:
        return "[(float)" + to_string(fval) + "]";
    case TK_ID:
        return "[(id)" + string(id) + "]";
    case TK_EOF:
        return "[<EOF>]";
    case KW_LOOP:
        return "[<LOOP>]";
    case KW_IF:
        return "[<IF>]";
    case KW_ELSE:
        return "[<ELSE>]";
    case KW_SHOT:
        return "[<SHOT>]";
    case KW_FUNC:
        return "[<FUNC>]";
    case KW_INT:
        return "[<INT>]";
    case KW_FLOAT:
        return "[<FLOAT>]";
    case KW_REF:
        return "[<REF>]";
    case KW_VAR:
        return "[<VAR>]";
    case KW_CASE:
        return "[<CASE>]";
    case KW_INIT:
        return "[<INIT>]";
    case KW_UPDATE:
        return "[<UPDATE>]";
    case KW_INTERRUPT:
        return "[<INTERRUPT>]";
    case KW_END:
        return "[<END>]";
    case OP_AND:
        return "[&&]";
    case OP_OR:
        return "[||]";
    case OP_EQ:
        return "[==]";
    case OP_NE:
        return "[!=]";
    case OP_GE:
        return "[>=]";
    case OP_LE:
        return "[<=]";
    case OP_INC:
        return "[++]";
    case OP_DEC:
        return "[--]";
    case OP_AA:
        return "[+=]";
    case OP_SA:
        return "[-=]";
    default:
        char buffer[16];
        sprintf(buffer, "[%c]", type);
        return buffer;
    }
}
