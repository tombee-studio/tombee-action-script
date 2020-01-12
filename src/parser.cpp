#include "../include/parser.hpp"

Parser*
Parser::getInstance() {
    static Parser *parser = new Parser();
    return parser;
}

Program *
Parser::parse(vector<Token>& tokens) {
    int pos = 0;
    Program *program = new Program();
    while(!consume(tokens, pos, Token::TK_EOF)) {
        Script *script = parse_script(tokens, pos);
        program->add(script->id(), script);
    }
    return program;
}

Script *
Parser::parse_script(vector<Token>& tokens, int& pos) {
    if(!consume(tokens, pos, Token::KW_FUNC)) throw ParseError("expected 'func'");
    Token *token;
    if(!(token = consume(tokens, pos, Token::TK_ID))) throw ParseError("expected '<ID>'");
    Script *script = new Script(token->id);
    if(!consume(tokens, pos, ':')) throw ParseError("expected ':'");
    while(!consume(tokens, pos, Token::KW_END)) script->add(parse_event(tokens, pos));
    return script;
}

Event *
Parser::parse_event(vector<Token>& tokens, int& pos) {
    Statement *statement;
    if(consume(tokens, pos, Token::KW_UPDATE)) {
        if(consume(tokens, pos, ':')) {
            return new UpdateEvent(parse_statement(tokens, pos));
        } else {
            throw ParseError("expected ':'");
        }
    } else if(consume(tokens, pos, Token::KW_INTERRUPT)) {
        Expression *cond = parse_expression(tokens, pos);
        if(consume(tokens, pos, ':')) {
            return new InterruptEvent(parse_statement(tokens, pos), cond);
        } else {
            throw ParseError("expected ':'");
        }
    } else if((statement = parse_statement(tokens, pos))) {
        return new InitEvent(statement);
    }
    return NULL;
}

Statement *
Parser::parse_block(vector<Token>& tokens, int& pos) {
    if(!consume(tokens, pos, '{')) return NULL;
    Block *block = new Block();
    while(!consume(tokens, pos, '}')) {
        block->add(parse_statement(tokens, pos));
    }
    return block;
}

Statement *
Parser::parse_ifst(vector<Token>& tokens, int& pos) {
    if(!consume(tokens, pos, Token::KW_IF)) return NULL;
    Expression *cond;
    Statement *truest;
    Statement *elsest;
    if(consume(tokens, pos, '(')) {
        cond = parse_expression(tokens, pos);
        if(!consume(tokens, pos, ')')) {
            throw ParseError("expected ')'");
        }
    } else {
        throw ParseError("expected '('");
    }
    truest = parse_statement(tokens, pos);
    if(!consume(tokens, pos, Token::KW_ELSE)) return new IfSt(cond, truest, NULL);
    elsest = parse_statement(tokens, pos);
    return new IfSt(cond, truest, elsest);
}

Statement *
Parser::parse_loop(vector<Token>& tokens, int& pos) {
    if(!consume(tokens, pos, Token::KW_LOOP)) return NULL;
    if(!consume(tokens, pos, '(')) throw ParseError("expected '('");
    Expression *times = parse_expression(tokens, pos);
    if(!consume(tokens, pos, ')')) throw ParseError("expected ')'");
    Statement  *body  = parse_statement(tokens, pos);
    return new Loop(times, body);
}

Statement *
Parser::parse_statement(vector<Token>& tokens, int& pos) {
    Statement *statement;
    Expression *exp;
    if((statement = parse_block(tokens, pos))) { return statement; }
    if((statement = parse_ifst(tokens, pos))) { return statement; }
    if((statement = parse_loop(tokens, pos))) { return statement; }
    if((exp = parse_expression(tokens, pos))) {
        if(consume(tokens, pos, ';')) {
            return new ExpressionSt(exp);
        } else {
            throw ParseError("expected ';'");
            exit(-1);
        }
    }
    return NULL;
}

Expression *
Parser::parse_expression(vector<Token>& tokens, int& pos) {
    return parse_assign(tokens, pos);
}

Expression *
Parser::parse_declare(vector<Token>& tokens, int& pos) {
    Token *token;
    if(consume(tokens, pos, Token::KW_INT)) {
        if((token = consume(tokens, pos, Token::TK_ID))) {
            return new Declare(Primitive::INT, token->id);
        } else {
            throw ParseError("expected <ID>");
        }
    } else if(consume(tokens, pos, Token::KW_FLOAT)) {
        if((token = consume(tokens, pos, Token::TK_ID))) {
            return new Declare(Primitive::FLOAT, token->id);
        } else {
            throw ParseError("expected <ID>");
        }
    }  else if(consume(tokens, pos, Token::KW_VAR)) {
        if((token = consume(tokens, pos, Token::TK_ID))) {
            return new Declare(Primitive::VAR, token->id);
        } else {
            throw ParseError("expected <ID>");
        }
    } else {
        return parse_equality(tokens, pos);
    }
}

Expression *
Parser::parse_assign(vector<Token>& tokens, int& pos) {
    Expression *exp = parse_declare(tokens, pos);
    if(consume(tokens, pos, '=')) {
        return new Assign(exp, parse_equality(tokens, pos));
    } else {
        return exp;
    }
}

Expression *
Parser::parse_equality(vector<Token>& tokens, int& pos) {
    Expression *exp = parse_compare(tokens, pos);
    if(consume(tokens, pos, Token::OP_EQ)) {
        return new EQExp(exp, parse_equality(tokens, pos));
    } else if(consume(tokens, pos, Token::OP_NE)) {
        return new NEExp(exp, parse_equality(tokens, pos));
    } else {
        return exp;
    }
}

Expression *
Parser::parse_compare(vector<Token>& tokens, int& pos) {
    Expression *exp = parse_add(tokens, pos);
    if(consume(tokens, pos, '<')) {
        return new LTExp(exp, parse_compare(tokens, pos));
    } else if(consume(tokens, pos, '>')) {
        return new GTExp(exp, parse_compare(tokens, pos));
    } else if(consume(tokens, pos, Token::OP_LE)) {
        return new LEExp(exp, parse_compare(tokens, pos));
    } else if(consume(tokens, pos, Token::OP_GE)) {
        return new GEExp(exp, parse_compare(tokens, pos));
    } else {
        return exp;
    }
}

Expression *
Parser::parse_add(vector<Token>& tokens, int& pos) {
    Expression *exp = parse_mul(tokens, pos);
    if(consume(tokens, pos, '+')) {
        return new AddExp(exp, parse_add(tokens, pos));
    } else if(consume(tokens, pos, '-')) {
        return new SubExp(exp, parse_add(tokens, pos));
    } else {
        return exp;
    }
}

Expression *
Parser::parse_mul(vector<Token>& tokens, int& pos) {
    Expression *exp = parse_unary(tokens, pos);
    if(consume(tokens, pos, '*')) {
        return new MulExp(exp, parse_mul(tokens, pos));
    } else if(consume(tokens, pos, '/')) {
        return new DivExp(exp, parse_mul(tokens, pos));
    } else {
        return exp;
    }
}

Expression *
Parser::parse_unary(vector<Token>& tokens, int& pos) {
    if(consume(tokens, pos, '-')) {
        return new MinusUnaryExp(parse_term(tokens, pos));
    }
    return parse_term(tokens, pos);
}

Expression *
Parser::parse_call(vector<Token>& tokens, int& pos) {
    Token *token;
    if((token = consume(tokens, pos, Token::TK_ID))) {
        if(consume(tokens, pos, '(')) {
            vector<Expression *> args;
            while(!consume(tokens, pos, ')')) {
                args.push_back(parse_expression(tokens, pos));
                if(consume(tokens, pos, ')')) break;
                if(!consume(tokens, pos, ',')) throw ParseError("expected ','");
            }
            return new CallExp(token->id, args);
        }
        throw ParseError("expected '('");
    } else {
        throw ParseError("expected 'ID'");
    }
}

Expression *
Parser::parse_term(vector<Token>& tokens, int& pos) {
    Token *token;
    if((token = consume(tokens, pos, Token::TK_INT))) {
        return new PrimitiveExp(token->ival);
    } else if((token = consume(tokens, pos, Token::TK_FLOAT))) {
        return new PrimitiveExp(token->fval);
    } else if((token = consume(tokens, pos, Token::TK_STRING))) {
        return new PrimitiveExp(token->id);
    } else if(tokens[pos].type == Token::TK_ID) {
        if(tokens[pos + 1].type == '(') {
            return parse_call(tokens, pos);
        } else if((token = consume(tokens, pos, Token::TK_ID))) {
            return new VarExp(token->id);
        }
    } else if(consume(tokens, pos, '(')) {
        Expression *exp = parse_expression(tokens, pos);
        if(exp) {
            if(consume(tokens, pos, ')')) {
                return exp;
            } else {
                throw ParseError("expected ')'");
            }
        } else {
            throw ParseError("expected <EXP>");
        }
    } 
    throw ParseError("expected <INT> or <FLOAT>");
}

Token * 
Parser::consume(vector<Token>& tokens, int& pos, int type) {
    if(tokens[pos].type == type) {
        return &tokens[pos++];
    } else {
        return NULL;
    }
}
