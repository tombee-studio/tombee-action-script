#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <cstdio>
#include "primitive.hpp"
#include "tac.hpp"
using namespace std;

class RuntimeError: public runtime_error {
public:
    RuntimeError(const char* message): runtime_error(message) {}
};

class CPU {
    void *_data;
    bool _isExit;
    int _pc;
    int _sp;
    map<string, Primitive (*)(CPU*, int, void*)> _functions;
    vector<map<string, Primitive>> _table;
    vector<int> loops;
    Primitive _stack[32];
    vector<TACOperand> _codes;
public:
    CPU(): _pc(0), _sp(1), _isExit(false) {}

    void run();
    bool isExit() { return _isExit; }
    void set(vector<TACOperand>& codes) { 
        _codes = codes; 
        _table.push_back(map<string, Primitive>()); 
    }
    void set(void *d) { _data = d; }
    void reserve(map<string, Primitive (*)(CPU*, int, void*)>& functions) { _functions = functions; }
    void print();
    void push(Primitive);
    Primitive pop();
    void start(int pc) { _pc = pc; _isExit = false; }
private:
    void expr(TACOperand);
    void load();
    void call(TACOperand);
    void je(TACOperand);
    void jne(TACOperand);
    void jmp(TACOperand);
    void assign();
    void rev();
    void declare(TACOperand);

    Primitive* find(string id);
};
