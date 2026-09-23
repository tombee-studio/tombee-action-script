#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <cstdint>
#include <fstream>
#include <sstream>
#include "tac.hpp"
#include "ast.hpp"

namespace tas {

struct BytecodeHeader {
    static constexpr uint32_t MAGIC = 0x54415342; // "TASB"
    static constexpr uint16_t VERSION = 1;

    uint32_t magic = MAGIC;
    uint16_t version = VERSION;
    uint16_t flags = 0;
    int32_t seq_entry_pc = -1;
    uint32_t event_count = 0;
    uint32_t interrupt_count = 0;
    uint32_t code_count = 0;
    uint32_t string_count = 0;
};

struct CompiledScriptData {
    string script_id;
    int seq_entry_pc = -1;
    map<string, int> event_entries;
    vector<pair<int, int>> interrupt_entries;
    vector<TACOperand> codes;
};

class BytecodeCompiler {
public:
    static bool compile_to_bytes(Script* script, vector<uint8_t>& out_bytes);
    static bool compile_to_file(Script* script, const string& filepath);
    static bool compile_program_to_file(Program* program, const string& script_name, const string& filepath);
};

class BytecodeLoader {
public:
    static bool load_from_bytes(const vector<uint8_t>& bytes, CompiledScriptData& out_data);
    static bool load_from_file(const string& filepath, CompiledScriptData& out_data);
};

} // namespace tas
