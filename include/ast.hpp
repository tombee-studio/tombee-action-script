#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <tuple>
#include "primitive.hpp"
#include "tac.hpp"

namespace tas {
using namespace std;

class Ast;
class Program;
class Block;

class Ast {
protected:
    virtual ~Ast() {}
public:
    virtual void print(int) = 0;
    virtual void print_tac() = 0;
    virtual void tac(vector<TACOperand>&, vector<int>&) = 0;
    void clear();
    void free() { delete this; }
protected:
    void tab(int);
};

class Statement: public Ast {
protected:
    virtual ~Statement() {}
public:
    virtual void print(int) = 0;
};

class Expression: public Ast {
protected:
    virtual ~Expression() {}
public:
    virtual void print(int) = 0;
    virtual void print_ltac() = 0;
    virtual void ltac(vector<TACOperand>&, vector<int>&) = 0;
};

class Event: public Ast {
protected:
    Statement  *_statement;

    virtual ~Event() { _statement->free(); }
public:
    Event(Statement *statement): _statement(statement) {}

    virtual void print(int) = 0;
    virtual void print_tac() = 0;
};

class CustomEvent: public Event {
    string _name;
public:
    CustomEvent(string name, Statement *statement): Event(statement), _name(name) {}
    virtual void print(int);
    virtual void print_tac();
    virtual void tac(vector<TACOperand>&, vector<int>&);

    string name() const { return _name; }
    Statement* statement() { return _statement; }
};

class InterruptEvent: public Event {
    Expression *_cond;
protected:
    virtual ~InterruptEvent() { _cond->free(); }
public:
    InterruptEvent(Statement *statement, Expression *cond): Event(statement), _cond(cond) {}
    virtual void print(int);
    virtual void print_tac();
    virtual void tac(vector<TACOperand>&, vector<int>&);

    Expression* cond() { return _cond; }
    Statement* statement() { return _statement; }
};

class InitEvent: public CustomEvent {
public:
    InitEvent(Statement *statement): CustomEvent("init", statement) {}
};

class UpdateEvent: public CustomEvent {
public:
    UpdateEvent(Statement *statement): CustomEvent("update", statement) {}
};

class RenderEvent: public CustomEvent {
public:
    RenderEvent(Statement *statement): CustomEvent("render", statement) {}
};

class SequentialCommand: public Ast {
    string _id;
    vector<Expression *> _args;
protected:
    virtual ~SequentialCommand() {
        for(auto arg: _args) {
            arg->free();
        }
        Ast::~Ast();
    }
public:
    SequentialCommand(string id, vector<Expression *> args): _id(id), _args(args) {}
    virtual void print(int);
    virtual void print_tac();
    virtual void tac(vector<TACOperand>&, vector<int>&);

    string id() const { return _id; }
    const vector<Expression *>& args() const { return _args; }
};

class Script: public Ast {
    string _id;
    vector<Ast *> _globals;
    vector<SequentialCommand *> _seq_commands;
    map<string, CustomEvent *> _events;
    vector<InterruptEvent *> _interrupts;

    int _seq_entry_pc;
    map<string, int> _event_entries;
    vector<pair<int, int>> _interrupt_entries;
protected:
    virtual ~Script();
public:
    Script(string id): _id(id), _seq_entry_pc(-1) {}

    virtual void print(int);
    virtual void print_tac();
    virtual void tac(vector<TACOperand>&, vector<int>&);

    void add_global(Ast *ast) { _globals.push_back(ast); }
    void add_sequential(SequentialCommand *cmd) { _seq_commands.push_back(cmd); }
    void add_event(CustomEvent *event) { _events[event->name()] = event; }
    void add_interrupt(InterruptEvent *intr) { _interrupts.push_back(intr); }

    void add(Event *event);

    string id() const { return _id; }
    int seq_entry_pc() const { return _seq_entry_pc; }
    const map<string, int>& event_entries() const { return _event_entries; }
    const vector<pair<int, int>>& interrupt_entries() const { return _interrupt_entries; }
    const vector<Ast *>& globals() const { return _globals; }
    const vector<SequentialCommand *>& seq_commands() const { return _seq_commands; }
    const map<string, CustomEvent *>& events() const { return _events; }
    const vector<InterruptEvent *>& interrupts() const { return _interrupts; }
};

class IfSt: public Statement {
    Expression *_cond;
    Statement *_truest;
    Statement *_elsest;
protected:
    virtual ~IfSt() {
        _cond->free();
        _truest->free();
        _elsest->free();
        Statement::~Statement();
    }
public:
    IfSt(Expression *cond, Statement *truest, Statement *elsest): _cond(cond), _truest(truest), _elsest(elsest) {}

    virtual void print(int);
    virtual void print_tac();
    virtual void tac(vector<TACOperand>&, vector<int>&);
};

class Loop: public Statement {
    Expression *_times;
    Statement  *_body;
public:
    Loop(Expression *times, Statement *body): _times(times), _body(body) {}
    virtual ~Loop() {
        _times->free();
        _body->free();
        Statement::~Statement();
    }

    virtual void print(int);
    virtual void print_tac();
    virtual void tac(vector<TACOperand>&, vector<int>&);
};

class Block: public Statement {
    vector<Ast*> _statements;
protected:
    virtual ~Block() {
        for(const auto statement: _statements) {
            statement->free();
        }
        Statement::~Statement();
    }
public:
    virtual void print(int);
    virtual void print_tac();
    virtual void tac(vector<TACOperand>&, vector<int>&);

    void add(Ast*);
};

class ExpressionSt: public Statement {
    Expression *_exp;
protected:
    virtual ~ExpressionSt() {
        _exp->free();
        Statement::~Statement();
    }
public:
    ExpressionSt(Expression *exp): _exp(exp) {}
    virtual void print(int tab);
    virtual void print_tac();
    virtual void tac(vector<TACOperand>&, vector<int>&);
};

class Declare: public Expression {
    int _type;
    string _id;
protected:
    virtual ~Declare() {}
public:
    Declare(int type, string id): _type(type), _id(id) {}
    virtual void print(int tab);
    virtual void print_tac();
    virtual void print_ltac();
    virtual void tac(vector<TACOperand>&, vector<int>&);
    virtual void ltac(vector<TACOperand>&, vector<int>&);
};

class Assign: public Expression {
    Expression *_left;
    Expression *_right;
protected:
    virtual ~Assign() { _left->free(); _right->free(); Expression::~Expression(); }
public:
    Assign(Expression *left, Expression *right): _left(left), _right(right) {}
    virtual void print(int tab);
    virtual void print_tac();
    virtual void print_ltac();
    virtual void tac(vector<TACOperand>&, vector<int>&);
    virtual void ltac(vector<TACOperand>&, vector<int>&);
};

class EQExp: public Expression {
    Expression *_left;
    Expression *_right;
protected:
    virtual ~EQExp() { _left->free(); _right->free(); Expression::~Expression(); }
public:
    EQExp(Expression *left, Expression *right): _left(left), _right(right) {}

    virtual void print(int tab);
    virtual void print_tac();
    virtual void print_ltac();
    virtual void tac(vector<TACOperand>&, vector<int>&);
    virtual void ltac(vector<TACOperand>&, vector<int>&);
};

class NEExp: public Expression {
    Expression *_left;
    Expression *_right;
protected:
    virtual ~NEExp() { _left->free(); _right->free(); Expression::~Expression(); }
public:
    NEExp(Expression *left, Expression *right): _left(left), _right(right) {}

    virtual void print(int tab);
    virtual void print_tac();
    virtual void print_ltac();
    virtual void tac(vector<TACOperand>&, vector<int>&);
    virtual void ltac(vector<TACOperand>&, vector<int>&);
};

class LTExp: public Expression {
    Expression *_left;
    Expression *_right;
protected:
    virtual ~LTExp() { _left->free(); _right->free(); Expression::~Expression(); }
public:
    LTExp(Expression *left, Expression *right): _left(left), _right(right) {}

    virtual void print(int tab);
    virtual void print_tac();
    virtual void print_ltac();
    virtual void tac(vector<TACOperand>&, vector<int>&);
    virtual void ltac(vector<TACOperand>&, vector<int>&);
};

class LEExp: public Expression {
    Expression *_left;
    Expression *_right;
protected:
    virtual ~LEExp() { _left->free(); _right->free(); Expression::~Expression(); }
public:
    LEExp(Expression *left, Expression *right): _left(left), _right(right) {}

    virtual void print(int tab);
    virtual void print_tac();
    virtual void print_ltac();
    virtual void tac(vector<TACOperand>&, vector<int>&);
    virtual void ltac(vector<TACOperand>&, vector<int>&);
};

class GTExp: public Expression {
    Expression *_left;
    Expression *_right;
protected:
    virtual ~GTExp() { _left->free(); _right->free(); Expression::~Expression(); }
public:
    GTExp(Expression *left, Expression *right): _left(left), _right(right) {}

    virtual void print(int tab);
    virtual void print_tac();
    virtual void print_ltac();
    virtual void tac(vector<TACOperand>&, vector<int>&);
    virtual void ltac(vector<TACOperand>&, vector<int>&);
};

class GEExp: public Expression {
    Expression *_left;
    Expression *_right;
protected:
    virtual ~GEExp() { _left->free(); _right->free(); Expression::~Expression(); }
public:
    GEExp(Expression *left, Expression *right): _left(left), _right(right) {}

    virtual void print(int tab);
    virtual void print_tac();
    virtual void print_ltac();
    virtual void tac(vector<TACOperand>&, vector<int>&);
    virtual void ltac(vector<TACOperand>&, vector<int>&);
};

class AddExp: public Expression {
    Expression *_left;
    Expression *_right;
protected:
    virtual ~AddExp() { _left->free(); _right->free(); Expression::~Expression(); }
public:
    AddExp(Expression *left, Expression *right): _left(left), _right(right) {}

    virtual void print(int tab);
    virtual void print_tac();
    virtual void print_ltac();
    virtual void tac(vector<TACOperand>&, vector<int>&);
    virtual void ltac(vector<TACOperand>&, vector<int>&);
};

class SubExp: public Expression {
    Expression *_left;
    Expression *_right;
protected:
    virtual ~SubExp() { _left->free(); _right->free(); Expression::~Expression(); }
public:
    SubExp(Expression *left, Expression *right): _left(left), _right(right) {}

    virtual void print(int tab);
    virtual void print_tac();
    virtual void print_ltac();
    virtual void tac(vector<TACOperand>&, vector<int>&);
    virtual void ltac(vector<TACOperand>&, vector<int>&);
};

class MulExp: public Expression {
    Expression *_left;
    Expression *_right;
protected:
    virtual ~MulExp() { _left->free(); _right->free(); Expression::~Expression(); }
public:
    MulExp(Expression *left, Expression *right): _left(left), _right(right) {}

    virtual void print(int tab);
    virtual void print_tac();
    virtual void print_ltac();
    virtual void tac(vector<TACOperand>&, vector<int>&);
    virtual void ltac(vector<TACOperand>&, vector<int>&);
};

class DivExp: public Expression {
    Expression *_left;
    Expression *_right;
protected:
    virtual ~DivExp() { _left->free(); _right->free(); Expression::~Expression(); }
public:
    DivExp(Expression *left, Expression *right): _left(left), _right(right) {}

    virtual void print(int tab);
    virtual void print_tac();
    virtual void print_ltac();
    virtual void tac(vector<TACOperand>&, vector<int>&);
    virtual void ltac(vector<TACOperand>&, vector<int>&);
};

class ModExp: public Expression {
    Expression *_left;
    Expression *_right;
protected:
    virtual ~ModExp() { _left->free(); _right->free(); Expression::~Expression(); }
public:
    ModExp(Expression *left, Expression *right): _left(left), _right(right) {}

    virtual void print(int tab);
    virtual void print_tac();
    virtual void print_ltac();
    virtual void tac(vector<TACOperand>&, vector<int>&);
    virtual void ltac(vector<TACOperand>&, vector<int>&);
};

class MinusUnaryExp: public Expression {
    Expression *_exp;
protected:
    virtual ~MinusUnaryExp() { _exp->free(); Expression::~Expression(); }
public:
    MinusUnaryExp(Expression *exp): _exp(exp) {}

    virtual void print(int tab);
    virtual void print_tac();
    virtual void print_ltac();
    virtual void tac(vector<TACOperand>&, vector<int>&);
    virtual void ltac(vector<TACOperand>&, vector<int>&);
};

class IncUnaryExp: public Expression {
    Expression *_exp;
protected:
    virtual ~IncUnaryExp() { _exp->free(); Expression::~Expression(); }
public:
    IncUnaryExp(Expression *exp): _exp(exp) {}

    virtual void print(int tab);
    virtual void print_tac();
    virtual void print_ltac();
    virtual void tac(vector<TACOperand>&, vector<int>&);
    virtual void ltac(vector<TACOperand>&, vector<int>&);
};

class DecUnaryExp: public Expression {
    Expression *_exp;
protected:
    virtual ~DecUnaryExp() { _exp->free(); Expression::~Expression(); }
public:
    DecUnaryExp(Expression *exp): _exp(exp) {}

    virtual void print(int tab);
    virtual void print_tac();
    virtual void print_ltac();
    virtual void tac(vector<TACOperand>&, vector<int>&);
    virtual void ltac(vector<TACOperand>&, vector<int>&);
};

class CallExp: public Expression {
    string _id;
    vector<Expression *> _args;
protected:
    virtual ~CallExp() { 
        for(auto arg: _args) {
            arg->free();
        }
        Expression::~Expression();
    }
public:
    CallExp(string id, vector<Expression *> args): _id(id), _args(args) {}

    virtual void print(int tab);
    virtual void print_tac();
    virtual void print_ltac();
    virtual void tac(vector<TACOperand>&, vector<int>&);
    virtual void ltac(vector<TACOperand>&, vector<int>&);
};

class PrimitiveExp: public Expression {
    Primitive value;
protected:
    virtual ~PrimitiveExp() {}
public:
    PrimitiveExp(int v) { this->value = Primitive::make_int(v); }
    PrimitiveExp(double v) { this->value = Primitive::make_float(v); }
    PrimitiveExp(string v) { this->value = Primitive::make_string(v); }

    virtual void print(int);
    virtual void print_tac();
    virtual void print_ltac();
    virtual void tac(vector<TACOperand>&, vector<int>&);
    virtual void ltac(vector<TACOperand>&, vector<int>&);
};

class VarExp: public Expression {
    string _id;
protected:
    virtual ~VarExp() {}
public:
    VarExp(string id): _id(id) {}
    virtual void print(int);
    virtual void print_tac();
    virtual void print_ltac();
    virtual void tac(vector<TACOperand>&, vector<int>&);
    virtual void ltac(vector<TACOperand>&, vector<int>&);
};

class Program {
    map<string, Ast*> _registered;
public:
    Program();
    ~Program();
    void print(int);
    tuple<vector<TACOperand>, vector<int>> tac(string);

    void add(string name, Ast *ast);
    Script* get_script(string name);
    const map<string, Ast*>& registered() const { return _registered; }
};

} // namespace tas
