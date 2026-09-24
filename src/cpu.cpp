#include "../include/cpu.hpp"
#include "../include/ast.hpp"

namespace tas {

CPU::CPU(): 
    _data(nullptr), 
    _isExit(false), 
    _pc(0), 
    _sp(0), 
    _seq_pc(-1), 
    _delay_ticks(0), 
    _seq_state(IDLE) 
{
}

void 
CPU::set(vector<TACOperand>& codes) { 
    set_codes(codes);
}

void
CPU::set_codes(const vector<TACOperand>& codes, const map<string, int>& event_entries, int seq_entry_pc, const vector<pair<int, int>>& interrupt_entries) {
    _codes = codes;
    _event_entries = event_entries;
    _seq_pc = seq_entry_pc;
    _interrupt_entries = interrupt_entries;
    _table.clear();
    _global_table.clear();
    _seq_state = (_seq_pc >= 0) ? IDLE : FINISHED;
    _delay_ticks = 0;
    _isExit = false;
    _pc = 0;
    _sp = 0;
    loops.clear();

    // Global変数の初期化コードがあれば実行 (この時は _table.empty() なので _global_table に保存される)
    if (!_codes.empty()) {
        int min_entry = (int)_codes.size();
        if (_seq_pc >= 0 && _seq_pc < min_entry) min_entry = _seq_pc;
        for (const auto& ev : _event_entries) {
            if (ev.second >= 0 && ev.second < min_entry) min_entry = ev.second;
        }
        for (const auto& intr : _interrupt_entries) {
            if (intr.first >= 0 && intr.first < min_entry) min_entry = intr.first;
        }
        if (min_entry > 0 && min_entry <= (int)_codes.size()) {
            _pc = 0;
            _sp = 0;
            _isExit = false;
            while (_pc < min_entry && !_isExit && _pc < (int)_codes.size()) {
                step();
            }
        }
    }
    _table.clear();
    _table.push_back(map<string, Primitive>());
    _pc = 0;
    _sp = 0;
    _isExit = false;
}

void
CPU::set_script(Script* script) {
    if (!script) return;
    vector<TACOperand> codes;
    vector<int> entries;
    script->tac(codes, entries);
    set_codes(codes, script->event_entries(), script->seq_entry_pc(), script->interrupt_entries());
}

void 
CPU::start(int pc) {
    _pc = pc;
    _sp = 0;
    _isExit = false;
    _table.clear();
    _table.push_back(map<string, Primitive>());
    loops.clear();
}

void
CPU::start_sequential() {
    if (_seq_pc >= 0) {
        _seq_state = RUNNING;
        _delay_ticks = 0;
    } else {
        _seq_state = FINISHED;
    }
}

void
CPU::step_sequential() {
    if (_seq_state == IDLE) {
        start_sequential();
    }
    if (_seq_state == SUSPENDED) {
        if (_delay_ticks > 0) {
            _delay_ticks--;
        }
        if (_delay_ticks <= 0) {
            _seq_state = RUNNING;
        }
    }
    if (_seq_state == RUNNING) {
        if (_table.empty()) {
            _table.push_back(map<string, Primitive>());
        }
        while (_seq_state == RUNNING && _seq_pc >= 0 && _seq_pc < (int)_codes.size()) {
            _pc = _seq_pc;
            TACOperand code = _codes[_pc];
            if (code.mnemonic == TACOperand::EXIT) {
                _seq_state = FINISHED;
                _seq_pc++;
                break;
            } else if (code.mnemonic == TACOperand::DELAY) {
                Primitive p = pop();
                _delay_ticks = (int)p;
                _seq_state = SUSPENDED;
                _seq_pc++;
                break;
            } else if (code.mnemonic == TACOperand::YIELD) {
                _delay_ticks = 1;
                _seq_state = SUSPENDED;
                _seq_pc++;
                break;
            } else if (code.mnemonic == TACOperand::DISPATCH) {
                Primitive p = pop();
                dispatch((string)p);
                _seq_pc++;
            } else {
                step();
                _seq_pc = _pc;
            }
        }
    }
}

void
CPU::dispatch(const string& event_name) {
    auto it = _event_entries.find(event_name);
    if (it == _event_entries.end() || it->second < 0 || it->second >= (int)_codes.size()) {
        return;
    }
    int save_pc = _pc;
    bool save_exit = _isExit;
    int save_sp = _sp;
    auto save_table = _table;
    auto save_loops = loops;

    start(it->second);
    while (!_isExit && _pc < (int)_codes.size()) {
        step();
    }

    _pc = save_pc;
    _isExit = save_exit;
    _sp = save_sp;
    _table = save_table;
    loops = save_loops;
}

void
CPU::check_interrupts() {
    for (const auto& intr : _interrupt_entries) {
        int cond_pc = intr.first;
        if (cond_pc < 0 || cond_pc >= (int)_codes.size()) continue;

        int save_pc = _pc;
        bool save_exit = _isExit;
        int save_sp = _sp;
        auto save_table = _table;
        auto save_loops = loops;

        start(cond_pc);
        while (!_isExit && _pc < (int)_codes.size()) {
            step();
        }

        _pc = save_pc;
        _isExit = save_exit;
        _sp = save_sp;
        _table = save_table;
        loops = save_loops;
    }
}

void
CPU::step() {
    if (_pc < 0 || _pc >= (int)_codes.size()) {
        _isExit = true;
        return;
    }
    TACOperand code = _codes[_pc];
    switch(code.mnemonic) {
    case TACOperand::EXIT:
        _isExit = true;
        break;
    case TACOperand::NEXT:
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
    case TACOperand::GLOBAL_DECL:
        declare_global((string)code.value);
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
    case TACOperand::NE:
    case TACOperand::LT:
    case TACOperand::LE:
    case TACOperand::GT:
    case TACOperand::GE:
    case TACOperand::ADD:
    case TACOperand::SUB:
    case TACOperand::MUL:
    case TACOperand::DIV:
    case TACOperand::MOD:
        expr(code);
        break;
    case TACOperand::REV:
        rev();
        break;
    case TACOperand::STAGED:
        _table.push_back(map<string, Primitive>());
        break;
    case TACOperand::UNSTAGED:
        if (_table.size() > 1) {
            _table.pop_back();
        }
        break;
    case TACOperand::LOOPSTART:
        loops.push_back((int)pop());
        break;
    case TACOperand::LOOPEND:
        if(!loops.empty() && loops.back() > 1) {
            _pc = (int)code.value;
            loops.back()--;
        } else if(!loops.empty()) {
            loops.pop_back();
        }
        break;
    case TACOperand::DELAY: {
        Primitive p = pop();
        _delay_ticks = (int)p;
        _seq_state = SUSPENDED;
        break;
    }
    case TACOperand::DISPATCH: {
        Primitive p = pop();
        dispatch((string)p);
        break;
    }
    case TACOperand::YIELD: {
        _delay_ticks = 1;
        _seq_state = SUSPENDED;
        break;
    }
    }
    _pc++;
}

void 
CPU::run() {
    step();
}

void 
CPU::call(TACOperand code) {
    auto it = _functions.find((string)code.value);
    if (it != _functions.end() && it->second != nullptr) {
        push(it->second(this, code.argsNum, _data));
    } else {
        push(Primitive::make_int(0));
    }
}

void 
CPU::print() {
    cout << "================" << endl;
    if (_pc >= 0 && _pc < (int)_codes.size()) {
        _codes[_pc].print();
    }
    cout << "--------" << endl;
    for(int i = 0; i < _sp; i++) {
        cout << i << ": " << (string)_stack[i] << endl;
    }
    cout << "---- Local Tables ----" << endl;
    for(const auto& table: _table) {
        for(const auto& var: table) {
            cout << var.first << ": " << (string)var.second << endl;
        }
    }
    cout << "---- Global Table ----" << endl;
    for(const auto& var: _global_table) {
        cout << var.first << ": " << (string)var.second << endl;
    }
    cout << "----------------" << endl;
}

void
CPU::declare_global(const string& id, Primitive val) {
    _global_table[id] = val;
}

void 
CPU::declare(TACOperand code) {
    if (_table.empty()) {
        _global_table[(string)code.value] = Primitive::make_none();
    } else {
        _table.back()[(string)code.value] = Primitive::make_none();
    }
}

void 
CPU::load() {
    Primitive p = pop();
    Primitive *value = find((string)p);
    if(value) {
        push(*value);
    } else {
        push(Primitive::make_int(0));
    }
}

void 
CPU::je(TACOperand code) {
    Primitive p = pop();
    if((int)p) {
        _pc = (int)code.value;
    }
}

void 
CPU::jne(TACOperand code) {
    Primitive p = pop();
    if(!(int)p) {
        _pc = (int)code.value;
    }
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
        if ((int)p1 == 0) {
            push(Primitive::make_int(0));
            return;
        }
        push(p2 / p1);
        return;
    case TACOperand::MOD:
        if ((int)p1 == 0) {
            push(Primitive::make_int(0));
            return;
        }
        push(p2 % p1);
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
        push(Primitive::make_int(0));
    }
}

void 
CPU::assign() {
    Primitive p1 = pop();
    Primitive p2 = pop();
    Primitive *target = find((string)p2);
    if(!target) {
        if (_table.empty()) {
            _global_table[(string)p2] = p1;
        } else {
            _table.back()[(string)p2] = p1;
        }
    } else {
        *target = p1;
    }
    push(p1);
}

void 
CPU::push(Primitive p) {
    if (_sp >= 512) {
        throw RuntimeError("stack overflow in TAS VM");
    }
    _stack[_sp++] = p;
}

Primitive 
CPU::pop() {
    if (_sp <= 0) {
        return Primitive::make_none();
    }
    _sp--;
    Primitive p = _stack[_sp];
    return p;
}

Primitive* 
CPU::find(string id) {
    for(int i = (int)_table.size() - 1; i >= 0; i--) {
        auto it = _table[i].find(id);
        if (it != _table[i].end()) {
            return &(it->second);
        }
    }
    auto it = _global_table.find(id);
    if (it != _global_table.end()) {
        return &(it->second);
    }
    return NULL;
}

} // namespace tas
