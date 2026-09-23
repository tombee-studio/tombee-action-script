#include "../include/parser.hpp"
#include "../include/cpu.hpp"
#include "../include/bytecode.hpp"
#include <cassert>

using namespace std;
using namespace tas;

Primitive
print(tas::CPU *cpu, int argsNum, void *data) {
    if (argsNum > 0) {
        tas::Primitive p = cpu->pop();
        cout << "[TAS Print] " << (string)p << endl;
    }
    return tas::Primitive::make_int(0);
}

Primitive
set_bullet(tas::CPU *cpu, int argsNum, void *data) {
    tas::Primitive count = (argsNum > 1) ? cpu->pop() : tas::Primitive::make_int(1);
    tas::Primitive name = (argsNum > 0) ? cpu->pop() : tas::Primitive::make_string("default");
    cout << "[TAS Action] set_bullet: " << (string)name << ", count=" << (int)count << endl;
    return tas::Primitive::make_int(1);
}

Primitive
m_random(tas::CPU *cpu, int argsNum, void *data) {
    tas::Primitive p = cpu->pop();
    int mod = (int)p;
    return tas::Primitive::make_int(mod > 0 ? (random() % mod) : 0);
}

int
main(int argc, char** argv) {
    if(argc > 1) {
        string file = argv[1];
        cout << "--- 1. Testing Lexer & Parser with file: " << file << " ---" << endl;
        vector<tas::Token> tokens = tas::Lexer::getInstance()->lex(file);
        tas::Program *program = tas::Parser::getInstance()->parse(tokens);

        tas::CPU cpu;
        map<string, tas::Primitive (*)(tas::CPU*, int, void*)> functions;
        functions["print"] = print;
        functions["Print"] = print;
        functions["set_bullet"] = set_bullet;
        functions["Random"] = m_random;
        cpu.reserve(functions);

        tas::Script* script = program->get_script("enemy001");
        assert(script != nullptr);
        cpu.set_script(script);

        cout << "--- 2. Testing Sequential Execution (@set_bullet, @delay, @dispatch) ---" << endl;
        // Step 1: @set_bullet
        cpu.step_sequential();
        // Step 2: @delay(2) -> suspended
        cpu.step_sequential();
        assert(cpu.get_seq_state() == tas::CPU::SUSPENDED);
        cout << "Suspended as expected. delay_ticks = " << cpu.get_delay_ticks() << endl;

        // Advance 2 ticks
        cpu.step_sequential(); // tick 1 -> 0
        cpu.step_sequential(); // resumes & executes @dispatch("custom_event")
        cout << "Sequential state after dispatch: " << cpu.get_seq_state() << endl;

        cout << "--- 3. Testing Event Dispatch ('init', 'update') ---" << endl;
        cpu.dispatch("init");
        cpu.dispatch("update");

        cout << "--- 4. Testing Bytecode Compilation & Loading (.tasb) ---" << endl;
        vector<uint8_t> bytes;
        bool compiled = tas::BytecodeCompiler::compile_to_bytes(script, bytes);
        assert(compiled && !bytes.empty());
        cout << "Compiled bytecode size: " << bytes.size() << " bytes" << endl;

        tas::CompiledScriptData loaded_data;
        bool loaded = tas::BytecodeLoader::load_from_bytes(bytes, loaded_data);
        assert(loaded);
        assert(loaded_data.codes.size() > 0);
        cout << "Successfully loaded bytecode. Code count = " << loaded_data.codes.size() << endl;

        tas::CPU bin_cpu;
        bin_cpu.reserve(functions);
        bin_cpu.set_codes(loaded_data.codes, loaded_data.event_entries, loaded_data.seq_entry_pc, loaded_data.interrupt_entries);
        bin_cpu.dispatch("custom_event");

        cout << "=== ALL TESTS PASSED! ===" << endl;
    }
    return 0;
}