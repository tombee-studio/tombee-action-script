#include "../include/parser.hpp"
#include "../include/cpu.hpp"

Primitive
print(CPU *cpu, int argsNum, void *data) {
    Primitive p = cpu->pop();
    cout << (string)p << endl;
    return Primitive::make_int(0);
}

Primitive
m_random(CPU *cpu, int argsNum, void *data) {
    Primitive p = cpu->pop();
    return Primitive::make_int(random() % (int)p);
}

int
main(int argc, char** argv) {
    if(argc > 0) {
        CPU cpu;
        map<string, Primitive (*)(CPU*, int, void*)> functions;
        functions["Print"] = print;
        functions["Random"] = m_random;
        vector<Token> tokens = Lexer::getInstance()->lex(argv[1]);
        Program *block = Parser::getInstance()->parse(tokens);
        auto [operands, entries] = block->tac("enemy001");
        cpu.reserve(functions);
        cpu.set(operands);
        while(!cpu.isExit()) { cpu.run(); }
    }
    return 0;
}