#include "../include/ast.hpp"

namespace tas {

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

Script*
Program::get_script(string name) {
    auto it = _registered.find(name);
    if (it != _registered.end()) {
        return dynamic_cast<Script*>(it->second);
    }
    return nullptr;
}

} // namespace tas
