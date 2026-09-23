#include "../include/bytecode.hpp"
#include <cstring>

namespace tas {

namespace {

class BinaryWriter {
    vector<uint8_t>& buf_;
public:
    BinaryWriter(vector<uint8_t>& buf) : buf_(buf) {}

    template<typename T>
    void write(const T& val) {
        const uint8_t* p = reinterpret_cast<const uint8_t*>(&val);
        buf_.insert(buf_.end(), p, p + sizeof(T));
    }

    void write_bytes(const void* data, size_t len) {
        const uint8_t* p = reinterpret_cast<const uint8_t*>(data);
        buf_.insert(buf_.end(), p, p + len);
    }
};

class BinaryReader {
    const uint8_t* data_;
    size_t size_;
    size_t pos_;
public:
    BinaryReader(const vector<uint8_t>& bytes) : data_(bytes.data()), size_(bytes.size()), pos_(0) {}

    template<typename T>
    bool read(T& val) {
        if (pos_ + sizeof(T) > size_) return false;
        std::memcpy(&val, data_ + pos_, sizeof(T));
        pos_ += sizeof(T);
        return true;
    }

    bool read_bytes(void* dest, size_t len) {
        if (pos_ + len > size_) return false;
        std::memcpy(dest, data_ + pos_, len);
        pos_ += len;
        return true;
    }

    bool has_more() const { return pos_ < size_; }
};

} // namespace

bool 
BytecodeCompiler::compile_to_bytes(Script* script, vector<uint8_t>& out_bytes) {
    if (!script) return false;

    vector<TACOperand> codes;
    vector<int> entries;
    script->tac(codes, entries);

    const auto& event_entries = script->event_entries();
    const auto& interrupt_entries = script->interrupt_entries();
    int seq_entry_pc = script->seq_entry_pc();

    // 1. 文字列プール構築
    vector<string> string_pool;
    map<string, uint32_t> string_map;

    auto intern_string = [&](const string& s) -> uint32_t {
        auto it = string_map.find(s);
        if (it != string_map.end()) return it->second;
        uint32_t idx = (uint32_t)string_pool.size();
        string_pool.push_back(s);
        string_map[s] = idx;
        return idx;
    };

    intern_string(script->id());
    for (const auto& ev : event_entries) {
        intern_string(ev.first);
    }
    for (const auto& op : codes) {
        if (op.value.type == Primitive::ID || op.value.type == Primitive::STRING) {
            intern_string((string)op.value);
        }
    }

    out_bytes.clear();
    BinaryWriter writer(out_bytes);

    // 2. ヘッダ書き込み
    BytecodeHeader header;
    header.magic = BytecodeHeader::MAGIC;
    header.version = BytecodeHeader::VERSION;
    header.flags = 0;
    header.seq_entry_pc = seq_entry_pc;
    header.event_count = (uint32_t)event_entries.size();
    header.interrupt_count = (uint32_t)interrupt_entries.size();
    header.code_count = (uint32_t)codes.size();
    header.string_count = (uint32_t)string_pool.size();

    writer.write(header);

    // 3. 文字列プール書き込み
    for (const auto& s : string_pool) {
        uint16_t len = (uint16_t)s.size();
        writer.write(len);
        if (len > 0) {
            writer.write_bytes(s.data(), len);
        }
    }

    // 4. イベントテーブル書き込み
    for (const auto& ev : event_entries) {
        uint32_t name_idx = string_map[ev.first];
        uint32_t pc = (uint32_t)ev.second;
        writer.write(name_idx);
        writer.write(pc);
    }

    // 5. 割り込みテーブル書き込み
    for (const auto& intr : interrupt_entries) {
        uint32_t cond_pc = (uint32_t)intr.first;
        uint32_t handler_pc = (uint32_t)intr.second;
        writer.write(cond_pc);
        writer.write(handler_pc);
    }

    // 6. コードセクション書き込み
    for (const auto& op : codes) {
        int32_t mnemonic = op.mnemonic;
        int32_t type = op.type;
        int32_t argsNum = op.argsNum;
        int32_t val_type = op.value.type;

        writer.write(mnemonic);
        writer.write(type);
        writer.write(argsNum);
        writer.write(val_type);

        if (val_type == Primitive::INT) {
            int32_t ival = op.value.int_val;
            writer.write(ival);
        } else if (val_type == Primitive::FLOAT) {
            double fval = op.value.float_val;
            writer.write(fval);
        } else if (val_type == Primitive::ID || val_type == Primitive::STRING) {
            uint32_t str_idx = string_map[(string)op.value];
            writer.write(str_idx);
        } else {
            int32_t dummy = 0;
            writer.write(dummy);
        }
    }

    return true;
}

bool 
BytecodeCompiler::compile_to_file(Script* script, const string& filepath) {
    vector<uint8_t> bytes;
    if (!compile_to_bytes(script, bytes)) return false;
    ofstream ofs(filepath, ios::binary);
    if (!ofs.is_open()) return false;
    ofs.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
    return ofs.good();
}

bool 
BytecodeCompiler::compile_program_to_file(Program* program, const string& script_name, const string& filepath) {
    if (!program) return false;
    Script* s = program->get_script(script_name);
    if (!s) return false;
    return compile_to_file(s, filepath);
}

bool 
BytecodeLoader::load_from_bytes(const vector<uint8_t>& bytes, CompiledScriptData& out_data) {
    BinaryReader reader(bytes);

    BytecodeHeader header;
    if (!reader.read(header)) return false;
    if (header.magic != BytecodeHeader::MAGIC) return false;
    if (header.version != BytecodeHeader::VERSION) return false;

    out_data.seq_entry_pc = header.seq_entry_pc;
    out_data.event_entries.clear();
    out_data.interrupt_entries.clear();
    out_data.codes.clear();

    // 1. 文字列プール読み込み
    vector<string> string_pool;
    string_pool.reserve(header.string_count);
    for (uint32_t i = 0; i < header.string_count; i++) {
        uint16_t len = 0;
        if (!reader.read(len)) return false;
        string s(len, '\0');
        if (len > 0) {
            if (!reader.read_bytes(&s[0], len)) return false;
        }
        string_pool.push_back(s);
    }

    if (!string_pool.empty()) {
        out_data.script_id = string_pool[0];
    }

    // 2. イベントテーブル読み込み
    for (uint32_t i = 0; i < header.event_count; i++) {
        uint32_t name_idx = 0;
        uint32_t pc = 0;
        if (!reader.read(name_idx) || !reader.read(pc)) return false;
        if (name_idx < string_pool.size()) {
            out_data.event_entries[string_pool[name_idx]] = (int)pc;
        }
    }

    // 3. 割り込みテーブル読み込み
    for (uint32_t i = 0; i < header.interrupt_count; i++) {
        uint32_t cond_pc = 0;
        uint32_t handler_pc = 0;
        if (!reader.read(cond_pc) || !reader.read(handler_pc)) return false;
        out_data.interrupt_entries.push_back({(int)cond_pc, (int)handler_pc});
    }

    // 4. コードセクション読み込み
    out_data.codes.reserve(header.code_count);
    for (uint32_t i = 0; i < header.code_count; i++) {
        int32_t mnemonic = 0;
        int32_t type = 0;
        int32_t argsNum = 0;
        int32_t val_type = 0;

        if (!reader.read(mnemonic) || !reader.read(type) || !reader.read(argsNum) || !reader.read(val_type)) {
            return false;
        }

        Primitive val;
        if (val_type == Primitive::INT) {
            int32_t ival = 0;
            if (!reader.read(ival)) return false;
            val = Primitive::make_int(ival);
        } else if (val_type == Primitive::FLOAT) {
            double fval = 0.0;
            if (!reader.read(fval)) return false;
            val = Primitive::make_float(fval);
        } else if (val_type == Primitive::ID) {
            uint32_t str_idx = 0;
            if (!reader.read(str_idx)) return false;
            val = Primitive::make_id(str_idx < string_pool.size() ? string_pool[str_idx] : "");
        } else if (val_type == Primitive::STRING) {
            uint32_t str_idx = 0;
            if (!reader.read(str_idx)) return false;
            val = Primitive::make_string(str_idx < string_pool.size() ? string_pool[str_idx] : "");
        } else {
            int32_t dummy = 0;
            if (!reader.read(dummy)) return false;
            val = Primitive::make_none();
        }

        TACOperand op = TACOperand::make(mnemonic, type, val, argsNum);
        out_data.codes.push_back(op);
    }

    return true;
}

bool 
BytecodeLoader::load_from_file(const string& filepath, CompiledScriptData& out_data) {
    ifstream ifs(filepath, ios::binary | ios::ate);
    if (!ifs.is_open()) return false;
    streamsize size = ifs.tellg();
    ifs.seekg(0, ios::beg);

    vector<uint8_t> bytes((size_t)size);
    if (!ifs.read(reinterpret_cast<char*>(bytes.data()), size)) return false;

    return load_from_bytes(bytes, out_data);
}

} // namespace tas
