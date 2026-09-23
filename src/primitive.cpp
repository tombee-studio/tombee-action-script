#include "../include/primitive.hpp"

namespace tas {

Primitive 
Primitive::make_int(int ival) {
    Primitive p;
    p.int_val = ival;
    p.type = INT;
    return p;
}

Primitive
Primitive::make_float(double fval) {
    Primitive p;
    p.float_val = fval;
    p.type = FLOAT;
    return p;
}

Primitive
Primitive::make_none() {
    Primitive p;
    p.type = NONE;
    return p;
}

Primitive
Primitive::make_array(Primitive* array, int size) {
    Primitive p;
    p.array = array;
    p.type = ARRAY;
    p.size = size;
    return p;
}

Primitive
Primitive::make_id(string id) {
    Primitive p;
    p._id = id;
    p.type = ID;
    return p;
}

Primitive
Primitive::make_string(string id) {
    Primitive p;
    p.str_value = id;
    p.type = STRING;
    return p;
}

Primitive 
Primitive::make_data(void *data) {
    Primitive p;
    p.data = data;
    p.type = VAR;
    return p;
}

Primitive& 
Primitive::operator>=(const Primitive& other) {
    bool res = false;
    if(this->type == other.type && this->type == STRING) {
        res = this->str_value >= other.str_value;
    } else {
        double v1 = (this->type == FLOAT) ? this->float_val : (double)(int)(*this);
        double v2 = (other.type == FLOAT) ? other.float_val : (double)(int)const_cast<Primitive&>(other);
        res = (v1 >= v2);
    }
    this->type = INT;
    this->int_val = res ? 1 : 0;
    return *this;
}

Primitive& 
Primitive::operator<=(const Primitive& other) {
    bool res = false;
    if(this->type == other.type && this->type == STRING) {
        res = this->str_value <= other.str_value;
    } else {
        double v1 = (this->type == FLOAT) ? this->float_val : (double)(int)(*this);
        double v2 = (other.type == FLOAT) ? other.float_val : (double)(int)const_cast<Primitive&>(other);
        res = (v1 <= v2);
    }
    this->type = INT;
    this->int_val = res ? 1 : 0;
    return *this;
}

Primitive& 
Primitive::operator>(const Primitive& other) {
    bool res = false;
    if(this->type == other.type && this->type == STRING) {
        res = this->str_value > other.str_value;
    } else {
        double v1 = (this->type == FLOAT) ? this->float_val : (double)(int)(*this);
        double v2 = (other.type == FLOAT) ? other.float_val : (double)(int)const_cast<Primitive&>(other);
        res = (v1 > v2);
    }
    this->type = INT;
    this->int_val = res ? 1 : 0;
    return *this;
}

Primitive& 
Primitive::operator<(const Primitive& other) {
    bool res = false;
    if(this->type == other.type && this->type == STRING) {
        res = this->str_value < other.str_value;
    } else {
        double v1 = (this->type == FLOAT) ? this->float_val : (double)(int)(*this);
        double v2 = (other.type == FLOAT) ? other.float_val : (double)(int)const_cast<Primitive&>(other);
        res = (v1 < v2);
    }
    this->type = INT;
    this->int_val = res ? 1 : 0;
    return *this;
}

Primitive& 
Primitive::operator!=(const Primitive& other) {
    bool res = false;
    if(this->type == other.type) {
        switch(this->type) {
        case INT: res = (this->int_val != other.int_val); break;
        case FLOAT: res = (this->float_val != other.float_val); break;
        case STRING: res = (this->str_value != other.str_value); break;
        case ID: res = (this->_id != other._id); break;
        default: res = (this->data != other.data); break;
        }
    } else {
        double v1 = (this->type == FLOAT) ? this->float_val : (double)(int)(*this);
        double v2 = (other.type == FLOAT) ? other.float_val : (double)(int)const_cast<Primitive&>(other);
        res = (v1 != v2);
    }
    this->type = INT;
    this->int_val = res ? 1 : 0;
    return *this;
}

Primitive& 
Primitive::operator==(const Primitive& other) {
    bool res = false;
    if(this->type == other.type) {
        switch(this->type) {
        case INT: res = (this->int_val == other.int_val); break;
        case FLOAT: res = (this->float_val == other.float_val); break;
        case STRING: res = (this->str_value == other.str_value); break;
        case ID: res = (this->_id == other._id); break;
        default: res = (this->data == other.data); break;
        }
    } else {
        double v1 = (this->type == FLOAT) ? this->float_val : (double)(int)(*this);
        double v2 = (other.type == FLOAT) ? other.float_val : (double)(int)const_cast<Primitive&>(other);
        res = (v1 == v2);
    }
    this->type = INT;
    this->int_val = res ? 1 : 0;
    return *this;
}

Primitive& 
Primitive::operator+(const Primitive& other) {
    if(this->type == STRING || other.type == STRING) {
        this->str_value = (string)(*this) + (string)const_cast<Primitive&>(other);
        this->type = STRING;
        return *this;
    }
    if(this->type == FLOAT || other.type == FLOAT) {
        double v1 = (this->type == FLOAT) ? this->float_val : (double)(int)(*this);
        double v2 = (other.type == FLOAT) ? other.float_val : (double)(int)const_cast<Primitive&>(other);
        this->type = FLOAT;
        this->float_val = v1 + v2;
        return *this;
    }
    int v1 = (int)(*this);
    int v2 = (int)const_cast<Primitive&>(other);
    this->type = INT;
    this->int_val = v1 + v2;
    return *this;
}

Primitive& 
Primitive::operator-(const Primitive& other) {
    if(this->type == FLOAT || other.type == FLOAT) {
        double v1 = (this->type == FLOAT) ? this->float_val : (double)(int)(*this);
        double v2 = (other.type == FLOAT) ? other.float_val : (double)(int)const_cast<Primitive&>(other);
        this->type = FLOAT;
        this->float_val = v1 - v2;
        return *this;
    }
    int v1 = (int)(*this);
    int v2 = (int)const_cast<Primitive&>(other);
    this->type = INT;
    this->int_val = v1 - v2;
    return *this;
}

Primitive& 
Primitive::operator*(const Primitive& other) {
    if(this->type == FLOAT || other.type == FLOAT) {
        double v1 = (this->type == FLOAT) ? this->float_val : (double)(int)(*this);
        double v2 = (other.type == FLOAT) ? other.float_val : (double)(int)const_cast<Primitive&>(other);
        this->type = FLOAT;
        this->float_val = v1 * v2;
        return *this;
    }
    int v1 = (int)(*this);
    int v2 = (int)const_cast<Primitive&>(other);
    this->type = INT;
    this->int_val = v1 * v2;
    return *this;
}

Primitive& 
Primitive::operator/(const Primitive& other) {
    if(this->type == FLOAT || other.type == FLOAT) {
        double v1 = (this->type == FLOAT) ? this->float_val : (double)(int)(*this);
        double v2 = (other.type == FLOAT) ? other.float_val : (double)(int)const_cast<Primitive&>(other);
        this->type = FLOAT;
        this->float_val = (v2 != 0.0) ? (v1 / v2) : 0.0;
        return *this;
    }
    int v1 = (int)(*this);
    int v2 = (int)const_cast<Primitive&>(other);
    this->type = INT;
    this->int_val = (v2 != 0) ? (v1 / v2) : 0;
    return *this;
}

Primitive& 
Primitive::operator%(const Primitive& other) {
    int v1 = (int)(*this);
    int v2 = (int)const_cast<Primitive&>(other);
    this->type = INT;
    this->int_val = (v2 != 0) ? (v1 % v2) : 0;
    return *this;
}

Primitive& 
Primitive::operator[](int index) {
    if(this->type == ARRAY && array != nullptr) {
        if(index >= 0 && index < size) {
            return array[index];
        }
    }
    static Primitive dummy = Primitive::make_none();
    return dummy;
}

Primitive::operator int() const {
    switch(type) {
    case INT: return int_val;
    case FLOAT: return (int)float_val;
    case STRING:
        try { return std::stoi(str_value); } catch(...) { return 0; }
    case ID:
        try { return std::stoi(_id); } catch(...) { return 0; }
    default: return 0;
    }
}

Primitive::operator double() const {
    switch(type) {
    case FLOAT: return float_val;
    case INT: return (double)int_val;
    case STRING:
        try { return std::stod(str_value); } catch(...) { return 0.0; }
    default: return 0.0;
    }
}

Primitive::operator unsigned char() const {
    return (unsigned char)(int)(*this);
}

Primitive::operator string() const {
    string str = "[";
    switch(type) {
    case ID:
        return _id;
    case ARRAY:
        for(int i = 0; i < size; i++) {
            str += (string)array[i];
        }
        str += " ]";
        return str;
    case INT:
        return to_string(int_val);
    case FLOAT:
        return to_string(float_val);
    case STRING:
        return str_value;
    case VAR:
        return "data";
    default:
        return "none";
    }
}

} // namespace tas
