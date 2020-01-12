#include "../include/ast.hpp"

void 
Ast::tab(int _t) {
    for(int i = 0; i < _t; i++) {
        cout << "  ";
    }
}

void
Ast::clear() {
    delete this;
}

tuple<vector<TACOperand>, vector<int>> 
Program::tac(string name) {
    vector<TACOperand> codes;
    vector<int> entries;
    _registered[name]->tac(codes, entries);
    return tuple<vector<TACOperand>, vector<int>>(codes, entries);
}

void 
InitEvent::print(int _tab) {
    _statement->print(_tab);
}

void 
InitEvent::print_tac() {
    cout << "init: " << endl;
    _statement->print_tac();
}

void 
InitEvent::tac(vector<TACOperand>& operands, vector<int>& entries) {
    if(entries.size() < 2) { entries.resize(2); }
    entries[0] = 0;
    _statement->tac(operands, entries);
}

void 
UpdateEvent::print(int _tab) {
    tab(_tab);
    cout << "update: ";
    _statement->print(_tab);
}

void 
UpdateEvent::print_tac() {
    cout << "update: " << endl;
    _statement->print_tac();
}

void 
UpdateEvent::tac(vector<TACOperand>& operands, vector<int>& entries) {
    if(entries.size() < 2) { entries.resize(2); }
    entries[1] = operands.size();
    _statement->tac(operands, entries);
    operands.push_back(TACOperand::make(TACOperand::EXIT, Primitive::NONE, Primitive::make_int(0)));
}

void 
InterruptEvent::print(int _tab) {
    tab(_tab);
    cout << "interrupt ";
    _cond->print(_tab);
    cout << ": ";
    _statement->print(_tab);
}

void 
InterruptEvent::print_tac() {
    cout << "interrupt: " << endl;
    _cond->print_tac();
    cout << "JE 2" << endl;
    cout << "EXIT" << endl;
    _statement->print_tac();
    cout << "EXIT" << endl;
}

void 
InterruptEvent::tac(vector<TACOperand>& operands, vector<int>& entries) {
    if(entries.size() < 2) { entries.resize(2); }
    entries.push_back(operands.size());
    _cond->tac(operands, entries);
    operands.push_back(TACOperand::make(TACOperand::JE, Primitive::NONE, Primitive::make_int(2)));
    operands.push_back(TACOperand::make(TACOperand::NEXT, Primitive::NONE, Primitive::make_int(0)));
    _statement->tac(operands, entries);
    operands.push_back(TACOperand::make(TACOperand::EXIT, Primitive::NONE, Primitive::make_int(0)));
}

void 
Script::print(int tab) {
    cout << "func " << _id << ": " << endl;
    for(auto event: _events) {
        event->print(tab + 1);
    }
    cout << "end" << endl;
}

void 
Script::print_tac() {
    cout << "func " << _id << ": " << endl;
    for(auto event: _events) {
        event->print_tac();
    }
}

void 
Script::tac(vector<TACOperand>& operands, vector<int>& entries) {
    for(auto event: _events) { event->tac(operands, entries); }
}

void 
IfSt::print(int _tab) {
    tab(_tab);
    cout << "if(";
    _cond->print(_tab);
    cout << ")";
    _truest->print(_tab);
    if(_elsest) {
        tab(_tab);
        cout << "else";
        _elsest->print(_tab);
    }
}

void 
IfSt::print_tac() {
}

void 
IfSt::tac(vector<TACOperand>& operands, vector<int>& entries) {
    operands.push_back(TACOperand::make(TACOperand::POP, Primitive::NONE, Primitive::make_int(0)));
    _cond->tac(operands, entries);
    operands.push_back(TACOperand::make(TACOperand::JNE, Primitive::NONE, Primitive::make_int(0)));
    int jumpTo = operands.size() - 1;
    _truest->tac(operands, entries);
    if(_elsest) {
        operands.push_back(TACOperand::make(TACOperand::JMP, Primitive::NONE, Primitive::make_int(0)));
        int jumpFrom = operands.size() - 1;
        _elsest->tac(operands, entries);
        operands[jumpTo].value = Primitive::make_int(jumpFrom);
        operands[jumpFrom].value = Primitive::make_int(operands.size() - 1);
    } else {
        operands[jumpTo].value = Primitive::make_int(operands.size() - 1);
    }
}

void 
Loop::print(int _tab) {
    tab(_tab);
    cout << "loop(";
    _times->print(_tab);
    cout << ")";
    _body->print(_tab);
}

void 
Loop::print_tac() {}

void 
Loop::tac(vector<TACOperand>& codes, vector<int>& entries) {
    codes.push_back(TACOperand::make(TACOperand::POP, Primitive::NONE, Primitive::make_int(0)));
    _times->tac(codes, entries);
    codes.push_back(TACOperand::make(TACOperand::LOOPSTART, Primitive::NONE, Primitive::make_none()));
    int top = codes.size() - 1;
    _body->tac(codes, entries);
    codes.push_back(TACOperand::make(TACOperand::LOOPEND, Primitive::NONE, Primitive::make_int(top)));
}

void 
ExpressionSt::print(int _tab) {
    tab(_tab);
    _exp->print(_tab);
    cout << ";" << endl;
}

void 
ExpressionSt::print_tac() {
    cout << "POP" << endl;
    _exp->print_tac();
}

void 
ExpressionSt::tac(vector<TACOperand>& operands, vector<int>& entries) {
    operands.push_back(TACOperand::make(TACOperand::POP, Primitive::NONE, Primitive::make_int(0)));
    _exp->tac(operands, entries);
}

void 
Declare::print(int tab) {
    switch(_type) {
    case Primitive::INT:
        cout << "int ";
        break;
    case Primitive::FLOAT:
        cout << "float ";
        break;
    case Primitive::VAR:
        cout << "var ";
        break;
    };
    cout << _id;
}

void 
Declare::print_tac() {

}

void 
Declare::print_ltac() {
    cout << "DECL ";
    switch(_type) {
    case Primitive::INT:
        cout << "INT ";
        break;
    case Primitive::FLOAT:
        cout << "FLOAT ";
        break;
    };
    cout << _id << endl;
    cout << "PUSH " << _id << endl;
}

void 
Declare::tac(vector<TACOperand>& operands, vector<int>& entries) {
    operands.push_back(TACOperand::make(TACOperand::DECL, _type, Primitive::make_id(_id)));
    operands.push_back(TACOperand::make(TACOperand::PUSH, Primitive::ID, Primitive::make_id(_id)));
}

void 
Declare::ltac(vector<TACOperand>& operands, vector<int>& entries) {
    operands.push_back(TACOperand::make(TACOperand::DECL, _type, Primitive::make_id(_id)));
    operands.push_back(TACOperand::make(TACOperand::PUSH, Primitive::ID, Primitive::make_id(_id)));
}

void
CallExp::print(int tab) {
    cout << _id;
    cout << "(";
    if(_args.size() > 0) {
        _args[0]->print(tab);
        for(int i = 1; i < _args.size(); i++) {
            cout << ",";
            _args[i]->print(tab);
        }
    }
    cout << ")";
}

void 
CallExp::print_tac() {
    for(int i = 0; i < _args.size(); i++) { _args[i]->print_tac(); }
    cout << "CALL " << _id << " " << _args.size() << endl;
    for(int i = 0; i < _args.size(); i++) { cout << "POP" << endl; }
    cout << "PUSH" << endl;
}

void 
CallExp::print_ltac() {
}

void 
CallExp::tac(vector<TACOperand>& operands, vector<int>& entries) {
    for(int i = _args.size() - 1; i >= 0; i--) { _args[i]->tac(operands, entries); }
    operands.push_back(TACOperand::make(TACOperand::CALL, Primitive::NONE, Primitive::make_id(_id), _args.size()));
}

void 
CallExp::ltac(vector<TACOperand>& operands, vector<int>& entries) {
}

void 
Assign::print(int tab) {
    _left->print(tab);
    cout << "=";
    _right->print(tab);
}

void 
Assign::print_tac() {
    _left->print_ltac();
    _right->print_tac();
    cout << "ASSIGN" << endl;
    cout << "PUSH" << endl;
}

void 
Assign::print_ltac() {
}

void 
Assign::tac(vector<TACOperand>& operands, vector<int>& entries) {
    _left->ltac(operands, entries);
    _right->tac(operands, entries);
    operands.push_back(TACOperand::make(TACOperand::ASSIGN, Primitive::NONE, Primitive::make_int(0)));
}

void 
Assign::ltac(vector<TACOperand>& operands, vector<int>& entries) {
}

void 
EQExp::print(int tab) {
    _left->print(tab);
    cout << "==";
    _right->print(tab);
}

void 
EQExp::print_tac() {
    _left->print_tac();
    _right->print_tac();
    cout << "EQ" << endl;
}

void 
EQExp::print_ltac() {
}

void 
EQExp::tac(vector<TACOperand>& operands, vector<int>& entries) {
    _left->tac(operands, entries);
    _right->tac(operands, entries);
    operands.push_back(TACOperand::make(TACOperand::EQ, Primitive::NONE, Primitive::make_int(0)));
}

void 
EQExp::ltac(vector<TACOperand>& operands, vector<int>& entries) {
}

void 
NEExp::print(int tab) {
    _left->print(tab);
    cout << "!=";
    _right->print(tab);
}

void 
NEExp::print_tac() {
    _left->print_tac();
    _right->print_tac();
    cout << "NE" << endl;
}

void 
NEExp::print_ltac() {
}

void 
NEExp::tac(vector<TACOperand>& operands, vector<int>& entries) {
    _left->tac(operands, entries);
    _right->tac(operands, entries);
    operands.push_back(TACOperand::make(TACOperand::NE, Primitive::NONE, Primitive::make_int(0)));
}

void 
NEExp::ltac(vector<TACOperand>& operands, vector<int>& entries) {
}

void 
LTExp::print(int tab) {
    _left->print(tab);
    cout << "<";
    _right->print(tab);
}

void 
LTExp::print_tac() {
    _left->print_tac();
    _right->print_tac();
    cout << "LT" << endl;
}

void 
LTExp::print_ltac() {
}

void 
LTExp::tac(vector<TACOperand>& operands, vector<int>& entries) {
    _left->tac(operands, entries);
    _right->tac(operands, entries);
    operands.push_back(TACOperand::make(TACOperand::LT, Primitive::NONE, Primitive::make_int(0)));
}

void 
LTExp::ltac(vector<TACOperand>& operands, vector<int>& entries) {
}

void 
LEExp::print(int tab) {
    _left->print(tab);
    cout << "<=";
    _right->print(tab);
}

void 
LEExp::print_tac() {
    _left->print_tac();
    _right->print_tac();
    cout << "LE" << endl;
}

void 
LEExp::print_ltac() {
}

void 
LEExp::tac(vector<TACOperand>& operands, vector<int>& entries) {
    _left->tac(operands, entries);
    _right->tac(operands, entries);
    operands.push_back(TACOperand::make(TACOperand::LE, Primitive::NONE, Primitive::make_int(0)));
}

void 
LEExp::ltac(vector<TACOperand>& operands, vector<int>& entries) {
}

void 
GTExp::print(int tab) {
    _left->print(tab);
    cout << ">";
    _right->print(tab);
}

void 
GTExp::print_tac() {
    _left->print_tac();
    _right->print_tac();
    cout << "GT" << endl;
}

void 
GTExp::print_ltac() {
}

void 
GTExp::tac(vector<TACOperand>& operands, vector<int>& entries) {
    _left->tac(operands, entries);
    _right->tac(operands, entries);
    operands.push_back(TACOperand::make(TACOperand::GT, Primitive::NONE, Primitive::make_int(0)));
}

void 
GTExp::ltac(vector<TACOperand>& operands, vector<int>& entries) {
}

void 
GEExp::print(int tab) {
    _left->print(tab);
    cout << ">=";
    _right->print(tab);
}

void 
GEExp::print_tac() {
    _left->print_tac();
    _right->print_tac();
    cout << "GE" << endl;
}

void 
GEExp::print_ltac() {
}

void 
GEExp::tac(vector<TACOperand>& operands, vector<int>& entries) {
    _left->tac(operands, entries);
    _right->tac(operands, entries);
    operands.push_back(TACOperand::make(TACOperand::GE, Primitive::NONE, Primitive::make_int(0)));
}

void 
GEExp::ltac(vector<TACOperand>& operands, vector<int>& entries) {
}

void 
AddExp::print(int tab) {
    _left->print(tab);
    cout << "+";
    _right->print(tab);
}

void 
AddExp::print_tac() {
    _left->print_tac();
    _right->print_tac();
    cout << "ADD" << endl;
}

void 
AddExp::print_ltac() {
}

void 
AddExp::tac(vector<TACOperand>& operands, vector<int>& entries) {
    _left->tac(operands, entries);
    _right->tac(operands, entries);
    operands.push_back(TACOperand::make(TACOperand::ADD, Primitive::NONE, Primitive::make_int(0)));
}

void 
AddExp::ltac(vector<TACOperand>& operands, vector<int>& entries) {
}


void 
SubExp::print(int tab) {
    _left->print(tab);
    cout << "-";
    _right->print(tab);
}

void 
SubExp::print_tac() {
    _left->print_tac();
    _right->print_tac();
    cout << "SUB" << endl;
}

void 
SubExp::print_ltac() {
}

void 
SubExp::tac(vector<TACOperand>& operands, vector<int>& entries) {
    _left->tac(operands, entries);
    _right->tac(operands, entries);
    operands.push_back(TACOperand::make(TACOperand::SUB, Primitive::NONE, Primitive::make_int(0)));
}

void 
SubExp::ltac(vector<TACOperand>& operands, vector<int>& entries) {
}

void 
MulExp::print(int tab) {
    _left->print(tab);
    cout << "*";
    _right->print(tab);
}

void 
MulExp::print_tac() {
    _left->print_tac();
    _right->print_tac();
    cout << "MUL" << endl;
}

void 
MulExp::tac(vector<TACOperand>& operands, vector<int>& entries) {
    _left->tac(operands, entries);
    _right->tac(operands, entries);
    operands.push_back(TACOperand::make(TACOperand::MUL, Primitive::NONE, Primitive::make_int(0)));
}

void 
MulExp::ltac(vector<TACOperand>& operands, vector<int>& entries) {
}

void 
MulExp::print_ltac() {
}

void 
DivExp::print(int tab) {
    _left->print(tab);
    cout << "/";
    _right->print(tab);
}

void 
DivExp::print_tac() {
    _left->print_tac();
    _right->print_tac();
    cout << "DIV" << endl;
}

void 
DivExp::print_ltac() {
}

void 
DivExp::tac(vector<TACOperand>& operands, vector<int>& entries) {
    _left->tac(operands, entries);
    _right->tac(operands, entries);
    operands.push_back(TACOperand::make(TACOperand::DIV, Primitive::NONE, Primitive::make_int(0)));
}

void 
DivExp::ltac(vector<TACOperand>& operands, vector<int>& entries) {
}

void 
MinusUnaryExp::print(int tab) {
    cout << "-";
    _exp->print(tab);
}

void 
MinusUnaryExp::print_tac() {
    _exp->print_tac();
    cout << "REV" << endl;
    cout << "PUSH" << endl;
}

void 
MinusUnaryExp::print_ltac() {
}

void 
MinusUnaryExp::tac(vector<TACOperand>& operands, vector<int>& entries) {
    _exp->tac(operands, entries);
    operands.push_back(TACOperand::make(TACOperand::REV, Primitive::NONE, Primitive::make_int(0)));
}

void 
MinusUnaryExp::ltac(vector<TACOperand>& operands, vector<int>& entries) {
}

void 
IncUnaryExp::print(int tab) {
    _exp->print(tab);
    cout << "++";
}

void 
IncUnaryExp::print_tac() {
    _exp->print_tac();
    cout << "REV" << endl;
    cout << "PUSH" << endl;
}

void 
IncUnaryExp::print_ltac() {
}

void 
IncUnaryExp::tac(vector<TACOperand>& operands, vector<int>& entries) {
    _exp->tac(operands, entries);
    operands.push_back(TACOperand::make(TACOperand::PUSH, Primitive::INT, Primitive::make_int(1)));
    operands.push_back(TACOperand::make(TACOperand::ADD, Primitive::NONE, Primitive::make_int(0)));
}

void 
IncUnaryExp::ltac(vector<TACOperand>& operands, vector<int>& entries) {
}

void 
DecUnaryExp::print(int tab) {
    _exp->print(tab);
    cout << "--";
}

void 
DecUnaryExp::print_tac() {
    _exp->print_tac();
    cout << "REV" << endl;
    cout << "PUSH" << endl;
}

void 
DecUnaryExp::print_ltac() {
}

void 
DecUnaryExp::tac(vector<TACOperand>& operands, vector<int>& entries) {
    _exp->tac(operands, entries);
    operands.push_back(TACOperand::make(TACOperand::PUSH, Primitive::INT, Primitive::make_int(1)));
    operands.push_back(TACOperand::make(TACOperand::SUB, Primitive::NONE, Primitive::make_int(0)));
}

void 
DecUnaryExp::ltac(vector<TACOperand>& operands, vector<int>& entries) {
}

void 
PrimitiveExp::print(int tab) {
    cout << (string)value;
}

void 
PrimitiveExp::print_tac() {
    cout << "PUSH " << (string)value << endl;
}

void 
PrimitiveExp::print_ltac() {
    cout << "PUSH " << (string)value << endl;
}

void 
PrimitiveExp::tac(vector<TACOperand>& operands, vector<int>& entries) {
    operands.push_back(TACOperand::make(TACOperand::PUSH, Primitive::NONE, value));
}

void 
PrimitiveExp::ltac(vector<TACOperand>& operands, vector<int>& entries) {
}

void 
VarExp::print(int tab) {
    cout << _id;
}

void 
VarExp::print_tac() {
    cout << "PUSH " << _id << endl;
    cout << "LOAD" << endl;
    cout << "PUSH" << endl;
}

void 
VarExp::print_ltac() {
    cout << "PUSH " << _id << endl;
}

void 
VarExp::tac(vector<TACOperand>& operands, vector<int>& entries) {
    operands.push_back(TACOperand::make(TACOperand::PUSH, Primitive::NONE, Primitive::make_id(_id)));
    operands.push_back(TACOperand::make(TACOperand::LOAD, Primitive::NONE, Primitive::make_int(0)));
}

void 
VarExp::ltac(vector<TACOperand>& operands, vector<int>& entries) {
    operands.push_back(TACOperand::make(TACOperand::PUSH, Primitive::NONE, Primitive::make_id(_id)));
}
