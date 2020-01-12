#include "../include/ast.hpp"

Program::Program() {}

Program::~Program() {
    for(const auto [key, ast]: _registered) {
        ast->clear();
    }
    _registered.clear();
}

void 
Program::print(int _t) {
    for(const auto [key, ast]: _registered) {
        ast->print(_t);
    }
}

void 
Program::add(string name, Ast* ast) {
    _registered[name] = ast;
}
