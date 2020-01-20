#ifndef _STRING_H_
#define _STIRNG_H_

#include <iostream>
#include <cstring>

struct string {
    unsigned int _capacity = 16;
    unsigned int _size;
    char *_str = NULL;

    string();
    string(const string&);
    string(const char *);
    ~string();

    string& operator=(const string&);
    string& operator=(const char*);

    string& operator+(const string&);
    string& operator+(const char*);
    string& operator+(const int&);
    string& operator+(const double&);
private:
    void set(const char *);
};

std::ostream& operator<<(std::ostream&, const string&);

#endif
