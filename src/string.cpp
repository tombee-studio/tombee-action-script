#include "../include/string.hpp"

string::string() {
    _capacity = 16;
    _size = 0;
    _str = (char *)malloc(_capacity * sizeof(char));
}

string::string(const string& other) {
    _capacity = other._capacity;
    _size = other._size;
    set(other._str);
}

string::string(const char *str) {
    _capacity = 16;
    while(_capacity < strlen(str)) _capacity *= 2;
    _size = strlen(str);
    set(str);
}

string::~string() {
    _capacity = 16;
    _size = 0;
    if(_str) free(_str);
}

string& 
string::operator=(const string &str) {
    _capacity = str._capacity;
    _size = str._size;
    set(str._str);
    return *this;
}

string& 
string::operator=(const char *str) {
    _capacity = 16;
    while(_capacity < strlen(str)) _capacity *= 2;
    _size = strlen(str);
    set(str);
    return *this;
}

void 
string::set(const char *str) {
    if(_str) {
        _str = (char *)realloc(_str, _capacity);
    } else {
        _str = (char *)malloc(_capacity);
    }
    strncpy(_str, str, _size);
}

string& 
string::operator+(const string &other) {
    char *start = _str + _size;
    while(_size > _capacity) _capacity *= 2;
    strcpy(start, other._str);
    _size = strlen(_str);
    return *this;
}

string& 
string::operator+(const int&) {
    return *this;
}

string& 
string::operator+(const double&) {
    return *this;
}

std::ostream&
operator<<(std::ostream &os, const string &str) {
    os << str._str;
    return os;
}
