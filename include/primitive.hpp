#pragma once

#include <cassert>
#define ASSERT(x) assert(x);

#include <iostream>
using namespace std;

struct Primitive {
    enum {
        NONE, INT, FLOAT, ARRAY, ID, STRING, VAR
    } type;

    union {
        int         int_val;
        double      float_val;
        Primitive*  array;
        void *data;
    };
    string _id;
    string str_value;

    int size;

    static Primitive make_int(int);
    static Primitive make_float(double);
    static Primitive make_none();
    static Primitive make_array(Primitive*, int);
    static Primitive make_id(string);
    static Primitive make_string(string);
    static Primitive make_data(void *);

    Primitive& operator!=(const Primitive& other);
    Primitive& operator==(const Primitive& other);
    Primitive& operator<(const Primitive& other);
    Primitive& operator>(const Primitive& other);
    Primitive& operator<=(const Primitive& other);
    Primitive& operator>=(const Primitive& other);
    Primitive& operator+(const Primitive& other);
    Primitive& operator-(const Primitive& other);
    Primitive& operator*(const Primitive& other);
    Primitive& operator/(const Primitive& other);
    Primitive& operator%(const Primitive& other);
    Primitive& operator[](int);

    operator int();
    operator double();
    operator string();
    operator unsigned char();
    template<class T>
    operator T() { return (T)data; }
};
