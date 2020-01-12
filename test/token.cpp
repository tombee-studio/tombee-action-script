#include "../include/token.hpp"

int
main() {
    Token token1 = Token::make_int(0);
    ASSERT(token1.ival == 0);
    Token token2 = Token::make_float(1.0);
    ASSERT(token2.fval == 1.0);
    return 0;
}
