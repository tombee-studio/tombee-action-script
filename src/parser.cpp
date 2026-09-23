#include "../include/parser.hpp"

namespace tas {

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

    while(!consume(tokens, pos, Token::KW_END)) {
        if(pos >= tokens.size() || tokens[pos].type == Token::TK_EOF) {
            throw ParseError("expected 'end'");
        }

        // 1. シーケンシャルコマンド: @command(...)
        if(consume(tokens, pos, Token::OP_AT)) {
            SequentialCommand *cmd = parse_sequential_command(tokens, pos);
            script->add_sequential(cmd);
            continue;
        }

        // 2. 割り込みイベント: interrupt <cond>: { ... }
        if(consume(tokens, pos, Token::KW_INTERRUPT)) {
            Expression *cond = parse_expression(tokens, pos);
            if(!consume(tokens, pos, ':')) throw ParseError("expected ':' after interrupt condition");
            Statement *body = parse_statement(tokens, pos);
            script->add_interrupt(new InterruptEvent(body, cond));
            continue;
        }

        // 3. カスタムイベント: <ID>: { ... }
        if(tokens[pos].type == Token::TK_ID && pos + 1 < tokens.size() && tokens[pos + 1].type == ':') {
            Token *ev_name = consume(tokens, pos, Token::TK_ID);
            consume(tokens, pos, ':');
            Statement *body = parse_statement(tokens, pos);
            script->add_event(new CustomEvent(ev_name->id, body));
            continue;
        }

        // 4. Global変数宣言または一般文
        if(tokens[pos].type == Token::KW_INT || tokens[pos].type == Token::KW_FLOAT || tokens[pos].type == Token::KW_VAR) {
            Expression *exp = parse_expression(tokens, pos);
            if(consume(tokens, pos, ';')) {
                script->add_global(new ExpressionSt(exp));
                continue;
            } else {
                throw ParseError("expected ';' after variable declaration");
            }
        }

        // 5. 一般文
        Statement *st = parse_statement(tokens, pos);
        if(st) {
            script->add_global(st);
        } else {
            throw ParseError("unexpected token in script body");
        }
    }
    return script;
}

SequentialCommand *
Parser::parse_sequential_command(vector<Token>& tokens, int& pos) {
    Token *token = consume(tokens, pos, Token::TK_ID);
    if(!token) throw ParseError("expected command name after '@'");
    vector<Expression *> args;
    if(consume(tokens, pos, '(')) {
        while(!consume(tokens, pos, ')')) {
            args.push_back(parse_expression(tokens, pos));
            if(consume(tokens, pos, ')')) break;
            if(!consume(tokens, pos, ',')) throw ParseError("expected ','");
        }
    }
    consume(tokens, pos, ';'); // セミコロンは省略可能
    return new SequentialCommand(token->id, args);
}

Event *
Parser::parse_event(vector<Token>& tokens, int& pos) {
    if(tokens[pos].type == Token::TK_ID && pos + 1 < tokens.size() && tokens[pos + 1].type == ':') {
        Token *ev_name = consume(tokens, pos, Token::TK_ID);
        consume(tokens, pos, ':');
        Statement *body = parse_statement(tokens, pos);
        return new CustomEvent(ev_name->id, body);
    } else if(consume(tokens, pos, Token::KW_INTERRUPT)) {
        Expression *cond = parse_expression(tokens, pos);
        if(consume(tokens, pos, ':')) {
            return new InterruptEvent(parse_statement(tokens, pos), cond);
        } else {
            throw ParseError("expected ':'");
        }
    } else {
        Statement *statement = parse_statement(tokens, pos);
        if(statement) {
            return new CustomEvent("init", statement);
        }
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
    } else if(consume(tokens, pos, '%')) {
        return new ModExp(exp, parse_mul(tokens, pos));
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

} // namespace tas
