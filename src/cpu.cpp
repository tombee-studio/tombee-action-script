#include "../include/cpu.hpp"

void 
CPU::run() {
    TACOperand code = _codes[_pc];
    switch(code.mnemonic) {
    case TACOperand::EXIT:
        _isExit = true;
        break;
    case TACOperand::PUSH:
        push(code.value);
        break;
    case TACOperand::POP:
        pop();
        break;
    case TACOperand::CALL:
        call(code);
        break;
    case TACOperand::ASSIGN:
        assign();
        break;
    case TACOperand::DECL:
        declare(code);
        break;
    case TACOperand::LOAD:
        load();
        break;
    case TACOperand::JE:
        je(code);
        break;
    case TACOperand::JNE:
        jne(code);
        break;
    case TACOperand::JMP:
        jmp(code);
        break;
    case TACOperand::EQ:
        expr(code);
        break;
    case TACOperand::NE:
        expr(code);
        break;
    case TACOperand::LT:
        expr(code);
        break;
    case TACOperand::LE:
        expr(code);
        break;
    case TACOperand::GT:
        expr(code);
        break;
    case TACOperand::GE:
        expr(code);
        break;
    case TACOperand::ADD:
        expr(code);
        break;
    case TACOperand::SUB:
        expr(code);
        break;
    case TACOperand::MUL:
        expr(code);
        break;
    case TACOperand::DIV:
        expr(code);
        break;
    case TACOperand::REV:
        rev();
        break;
    case TACOperand::STAGED:
        _table.push_back(map<string, Primitive>());
        break;
    case TACOperand::UNSTAGED:
        _table.pop_back();
        break;
    case TACOperand::LOOPSTART:
        loops.push_back(pop());
        push(Primitive::make_none());
        break;
    case TACOperand::LOOPEND:
        if(loops[loops.size() - 1] > 1) {
            _pc = code.value;
            loops[loops.size() - 1]--;
        } else {
            loops.pop_back();
        }
        break;
    }
    _pc++;
}

void 
CPU::call(TACOperand code) {
    try {
        push(_functions.at((string)code.value)(this, code.argsNum, _data));
    } catch(out_of_range err) {
        throw RuntimeError("can't find function name");
    }
}

void 
CPU::print() {
    cout << "================" << endl;
    _codes[_pc].print();
    cout << "--------" << endl;
    for(int i = 0; i < _sp; i++) {
        cout << i << ": " << (string)_stack[i] << endl;
    }
    cout << "--------" << endl;
    for(auto table: _table) {
        for(auto var: table) {
            cout << var.first << ": " << (string)var.second << endl;
        }
    }
    cout << "----------------" << endl;
}

void 
CPU::declare(TACOperand code) {
    if(!_table.size()) throw RuntimeError("variable table was not initialized");
    _table[_table.size() - 1][(string)code.value] = Primitive::make_none();
}

void 
CPU::load() {
    Primitive p = pop();
    Primitive *value = find(p);
    if(value) {
        push(*value);
        return;
    } else {
        throw RuntimeError("undefined variable");
    }
}

void 
CPU::je(TACOperand code) {
    Primitive p = pop();
    if((int)p) {
        _pc = (int)code.value;
    }
    push(p);
}

void 
CPU::jne(TACOperand code) {
    Primitive p = pop();
    if(!(int)p) {
        _pc = (int)code.value;
    }
    push(p);
}

void 
CPU::jmp(TACOperand code) {
    _pc = (int)code.value;
}

void 
CPU::expr(TACOperand code) {
    Primitive p1 = pop();
    Primitive p2 = pop();
    switch(code.mnemonic) {
    case TACOperand::EQ:
        push(p2 == p1);
        return;
    case TACOperand::NE:
        push(p2 != p1);
        return;
    case TACOperand::LT:
        push(p2 < p1);
        return;
    case TACOperand::LE:
        push(p2 <= p1);
        return;
    case TACOperand::GT:
        push(p2 > p1);
        return;
    case TACOperand::GE:
        push(p2 >= p1);
        return;
    case TACOperand::ADD:
        push(p2 + p1);
        return;
    case TACOperand::SUB:
        push(p2 - p1);
        return;
    case TACOperand::MUL:
        push(p2 * p1);
        return;
    case TACOperand::DIV:
        push(p2 / p1);
        return;
    }
}

void 
CPU::rev() {
    Primitive p = pop();
    if(p.type == Primitive::INT) {
        p.int_val *= -1;
        push(p);
    } else if(p.type == Primitive::FLOAT) {
        p.float_val *= -1.0;
        push(p);
    } else {
        throw RuntimeError("can't reverse");
    }
}

void 
CPU::assign() {
    Primitive p1 = pop();
    Primitive p2 = pop();
    Primitive *target = find((string)p2);
    if(!target) throw RuntimeError("undeclared variable");
    *target = p1;
    push(p1);
}

void 
CPU::push(Primitive p) {
    _stack[_sp++] = p;
}

Primitive 
CPU::pop() {
    _sp--;
    Primitive p = _stack[_sp];
    return p;
}

Primitive* 
CPU::find(string id) {
    for(int i = 0; i < _table.size(); i++) {
        try {
            return &_table[i].at(id);
        } catch(out_of_range err) {

        }
    }
    return NULL;
}
