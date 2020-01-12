#include "../include/lexer.hpp"

int
main(int argc, char** argv) {
    if(argc > 0) {
        vector<Token> tokens = Lexer::getInstance()->lex(argv[1]);
    }
    return 0;
}
