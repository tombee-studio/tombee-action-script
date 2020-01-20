#include "../include/string.hpp"

int
main() {
    string str("Hello, World. ");
    string str1 = "I am a hero.";
    string str2 = " This is a pen.";
    string str3 = str + str1 + str2;
    std::cout << str3 << std::endl;
    return 0;
}
