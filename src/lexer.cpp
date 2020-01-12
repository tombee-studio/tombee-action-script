#include "../include/lexer.hpp"

Lexer* 
Lexer::getInstance() {
    static Lexer *lexer = new Lexer();
    return lexer;
}

vector<Token>
Lexer::lex(string fileName) {
    tokens.clear();
    string line;
    ifstream ifs(fileName);
    if(ifs.fail()) {
        throw runtime_error("can't find file");
    }
    while(getline(ifs, line)) {
        int pos = 0;
        while(pos < line.size()) {
            tokenize(line, pos);
        }
    }
    ifs.close();

    tokens.push_back(Token::make_token(Token::TK_EOF));

    return tokens;
}

void
Lexer::tokenize(string& line, int& pos) {
    if(skip(line, pos)) return;
    if(comment(line, pos)) return;
    if(tokenizeOperator(line, pos, Token::OP_AND, "&&")) return;
    if(tokenizeOperator(line, pos, Token::OP_OR, "||")) return;
    if(tokenizeOperator(line, pos, Token::OP_EQ, "==")) return;
    if(tokenizeOperator(line, pos, Token::OP_NE, "!=")) return;
    if(tokenizeOperator(line, pos, Token::OP_GE, ">=")) return;
    if(tokenizeOperator(line, pos, Token::OP_LE, "<=")) return;
    if(tokenizeOperator(line, pos, Token::OP_AA, "+=")) return;
    if(tokenizeOperator(line, pos, Token::OP_SA, "-=")) return;
    if(tokenizeOperator(line, pos, Token::OP_INC, "++")) return;
    if(tokenizeOperator(line, pos, Token::OP_DEC, "--")) return;
    if(tokenizeOperator(line, pos, '<')) return;
    if(tokenizeOperator(line, pos, '>')) return;
    if(tokenizeOperator(line, pos, '+')) return;
    if(tokenizeOperator(line, pos, '-')) return;
    if(tokenizeOperator(line, pos, '*')) return;
    if(tokenizeOperator(line, pos, '/')) return;
    if(tokenizeOperator(line, pos, '%')) return;
    if(tokenizeOperator(line, pos, '=')) return;
    if(tokenizeOperator(line, pos, '@')) return;
    if(tokenizeOperator(line, pos, '{')) return;
    if(tokenizeOperator(line, pos, '}')) return;
    if(tokenizeOperator(line, pos, '(')) return;
    if(tokenizeOperator(line, pos, ')')) return;
    if(tokenizeOperator(line, pos, ':')) return;
    if(tokenizeOperator(line, pos, ',')) return;
    if(tokenizeOperator(line, pos, ';')) return;
    if(tokenizeKeyword(line, pos, Token::KW_IF, "if")) return;
    if(tokenizeKeyword(line, pos, Token::KW_ELSE, "else")) return;
    if(tokenizeKeyword(line, pos, Token::KW_FUNC, "func")) return;
    if(tokenizeKeyword(line, pos, Token::KW_LOOP, "loop")) return;
    if(tokenizeKeyword(line, pos, Token::KW_SHOT, "shot")) return;
    if(tokenizeKeyword(line, pos, Token::KW_CASE, "case")) return;
    if(tokenizeKeyword(line, pos, Token::KW_INIT, "init")) return;
    if(tokenizeKeyword(line, pos, Token::KW_UPDATE, "update")) return;
    if(tokenizeKeyword(line, pos, Token::KW_INTERRUPT, "interrupt")) return;
    if(tokenizeKeyword(line, pos, Token::KW_INT, "int")) return;
    if(tokenizeKeyword(line, pos, Token::KW_FLOAT, "float")) return;
    if(tokenizeKeyword(line, pos, Token::KW_REF, "ref")) return;
    if(tokenizeKeyword(line, pos, Token::KW_VAR, "var")) return;
    if(tokenizeKeyword(line, pos, Token::KW_END, "end")) return;
    if(tokenizeId(line, pos)) return;
    if(tokenizeNum(line, pos)) return;
    if(tokenizeStr(line, pos)) return;
    fprintf(stderr, "**ERROR** unexpected char: '%c'\n", line[pos]);
    exit(-1);
}

bool 
Lexer::skip(string& line, int& pos) {
    if(line[pos] == ' ') {
        pos++;
        return true;
    }
    return false;
}

bool
Lexer::comment(string& line, int& pos) {
    if(line[pos] == '/' && line[pos+1] == '/') {
        pos = line.size();
        return true;
    }
    return false;
}

bool
Lexer::tokenizeOperator(string& line, int& pos, char c) {
    if(line[pos] != c) return false;
    Token token;
    token.type = c;
    tokens.push_back(token);
    pos++;
    return true;
}

bool
Lexer::tokenizeOperator(string& line, int& pos, int type, string op) {
    int cur = pos;
    for(const auto c: op) {
        if(line[cur++] != c) return false;
    }
    Token token;
    token.type = type;
    tokens.push_back(token);
    pos = cur;
    return true;
}

bool
Lexer::tokenizeKeyword(string& line, int& pos, int type, string keyword) {
    int cur = pos;
    for(const auto c: keyword) { if(line[cur++] != c) return false; }
    if(isalnum(line[cur])) return false;
    Token token;
    token.type = type;
    tokens.push_back(token);
    pos = cur;
    return true;
}

bool
Lexer::tokenizeId(string& line, int& pos) {
    int start = pos;
    if(!isalpha(line[start]) && line[start] != '_') return false;
    while(isalnum(line[pos]) || line[pos] == '_') pos++;
    tokens.push_back(Token::make_id(line.substr(start, pos - start)));
    return true;
}

bool
Lexer::tokenizeNum(string& line, int& pos) {
    int start = pos;
    if(!isdigit(line[start]) && line[start] != '_') return false;
    Token token;
    token.type = Token::TK_INT;
    while(isdigit(line[++pos]) || line[pos] == '.') {
        if(line[pos] == '.') token.type = Token::TK_FLOAT;
    }
    if(token.type == Token::TK_INT) {
        tokens.push_back(Token::make_int(stoi(line.substr(start, pos - start))));
    } else if(token.type == Token::TK_FLOAT) {
        tokens.push_back(Token::make_float(stod(line.substr(start, pos - start))));
    } else {
        fprintf(stderr, "**ERROR** unexpected type\n");
        exit(-1);
    }
    return true;
}

bool
Lexer::tokenizeStr(string& line, int& pos) {
    if(line[pos++] != '\"') return false;
    int start = pos;
    while(line[pos++] != '\"');
    int end = pos - 1;
    Token token;
    token.type = Token::TK_STRING;
    token.id = line.substr(start, end - start);
    tokens.push_back(token);
    return true;
}
