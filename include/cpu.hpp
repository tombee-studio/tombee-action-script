#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <cstdio>
#include <stdexcept>
#include "primitive.hpp"
#include "tac.hpp"

namespace tas {
using namespace std;

class Script;

class RuntimeError: public runtime_error {
public:
    RuntimeError(const string& message): runtime_error(message) {}
    RuntimeError(const char* message): runtime_error(message) {}
};

class CPU {
public:
    enum State {
        IDLE,
        RUNNING,
        SUSPENDED,
        FINISHED
    };

private:
    void *_data;
    bool _isExit;
    int _pc;
    int _sp;
    map<string, Primitive (*)(CPU*, int, void*)> _functions;
    map<string, Primitive> _global_table;
    vector<map<string, Primitive>> _table;
    vector<int> loops;
    Primitive _stack[512];
    vector<TACOperand> _codes;

    map<string, int> _event_entries;
    vector<pair<int, int>> _interrupt_entries;
    int _seq_pc;
    int _delay_ticks;
    State _seq_state;

public:
    CPU();

    void run();
    void step();
    bool isExit() const { return _isExit; }

    void set(vector<TACOperand>& codes);
    void set_codes(const vector<TACOperand>& codes, const map<string, int>& event_entries = {}, int seq_entry_pc = -1, const vector<pair<int, int>>& interrupt_entries = {});
    void set_script(Script* script);
    void set(void *d) { _data = d; }
    void reserve(const map<string, Primitive (*)(CPU*, int, void*)>& functions) { _functions = functions; }

    void print();
    void push(Primitive);
    Primitive pop();

    void start(int pc);
    void start_sequential();
    void step_sequential();
    void dispatch(const string& event_name);
    void check_interrupts();

    State get_seq_state() const { return _seq_state; }
    bool is_seq_finished() const { return _seq_state == FINISHED; }
    int get_delay_ticks() const { return _delay_ticks; }
    const vector<TACOperand>& get_codes() const { return _codes; }
    const map<string, int>& get_event_entries() const { return _event_entries; }
    int get_seq_pc() const { return _seq_pc; }
    const vector<pair<int, int>>& get_interrupt_entries() const { return _interrupt_entries; }

    void declare_global(const string& id, Primitive val = Primitive::make_none());
    Primitive* find(string id);

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
};

} // namespace tas
