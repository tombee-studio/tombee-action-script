#include "../include/ast.hpp"

void
Block::print(int _t) {
    cout << "{" << endl;
    for(const auto statement: _statements) {
        statement->print(_t+1);
    }
    tab(_t);
    cout << "}" << endl;
}

void 
Block::print_tac() {
    for(const auto statement: _statements) {
        statement->print_tac();
    }
}

void 
Block::tac(vector<TACOperand>& operands, vector<int>& entries) {
    operands.push_back(TACOperand::make(TACOperand::STAGED, Primitive::NONE, Primitive::make_int(0)));
    for(const auto statement: _statements) {
        statement->tac(operands, entries);
    }
    operands.push_back(TACOperand::make(TACOperand::UNSTAGED, Primitive::NONE, Primitive::make_int(0)));
}

void
Block::add(Ast* ast) {
    _statements.push_back(ast);
}