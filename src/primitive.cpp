#include "../include/primitive.hpp"

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
    if(this->type == other.type) {
        switch(this->type) {
        case INT:
            this->int_val = this->int_val >= other.int_val;
            return *this;
        case FLOAT:
            this->type = INT;
            this->float_val = this->float_val >= other.float_val;
            return *this;
        default:
            cerr << "**ERROR** can't add." << endl;
            exit(-1);
        }
    } else {
        cerr << "**ERROR** Both values must be same to add." << endl;
        exit(-1);
    }
}

Primitive& 
Primitive::operator<=(const Primitive& other) {
    if(this->type == other.type) {
        switch(this->type) {
        case INT:
            this->int_val = this->int_val <= other.int_val;
            return *this;
        case FLOAT:
            this->type = INT;
            this->float_val = this->float_val <= other.float_val;
            return *this;
        default:
            cerr << "**ERROR** can't add." << endl;
            exit(-1);
        }
    } else {
        cerr << "**ERROR** Both values must be same to add." << endl;
        exit(-1);
    }
}

Primitive& 
Primitive::operator>(const Primitive& other) {
    if(this->type == other.type) {
        switch(this->type) {
        case INT:
            this->int_val = this->int_val > other.int_val;
            return *this;
        case FLOAT:
            this->type = INT;
            this->float_val = this->float_val > other.float_val;
            return *this;
        default:
            cerr << "**ERROR** can't add." << endl;
            exit(-1);
        }
    } else {
        cerr << "**ERROR** Both values must be same to add." << endl;
        exit(-1);
    }
}

Primitive& 
Primitive::operator<(const Primitive& other) {
    if(this->type == other.type) {
        switch(this->type) {
        case INT:
            this->int_val = this->int_val < other.int_val;
            return *this;
        case FLOAT:
            this->type = INT;
            this->float_val = this->float_val < other.float_val;
            return *this;
        default:
            cerr << "**ERROR** can't add." << endl;
            exit(-1);
        }
    } else {
        cerr << "**ERROR** Both values must be same to add." << endl;
        exit(-1);
    }
}

Primitive& 
Primitive::operator!=(const Primitive& other) {
    if(this->type == other.type) {
        switch(this->type) {
        case INT:
            this->int_val = this->int_val != other.int_val;
            return *this;
        case FLOAT:
            this->type = INT;
            this->float_val = this->float_val != other.float_val;
            return *this;
        default:
            cerr << "**ERROR** can't add." << endl;
            exit(-1);
        }
    } else {
        cerr << "**ERROR** Both values must be same to add." << endl;
        exit(-1);
    }
}

Primitive& 
Primitive::operator==(const Primitive& other) {
    if(this->type == other.type) {
        switch(this->type) {
        case INT:
            this->int_val = this->int_val == other.int_val;
            return *this;
        case FLOAT:
            this->type = INT;
            this->float_val = this->float_val == other.float_val;
            return *this;
        default:
            cerr << "**ERROR** can't add." << endl;
            exit(-1);
        }
    } else {
        cerr << "**ERROR** Both values must be same to add." << endl;
        exit(-1);
    }
}

Primitive& 
Primitive::operator+(const Primitive& other) {
    Primitive p = other;
    if(this->type == other.type) {
        switch(this->type) {
        case INT:
            this->int_val += other.int_val;
            return *this;
        case FLOAT:
            this->float_val += other.float_val;
            return *this;
        case STRING:
            this->str_value += other.str_value;
            return *this;
        default:
            cerr << "**ERROR** can't add." << endl;
            exit(-1);
        }
    } else {
        if(this->type == INT && other.type == FLOAT) {
            this->type = FLOAT;
            this->float_val = p.float_val + (double)this->int_val;
            return *this;
        } else if(this->type == FLOAT && other.type == INT) {
            this->float_val += (double)other.int_val;
            return *this;
        }
        if(this->type == STRING) {
            this->str_value += (string)p;
            return *this;
        } else if(other.type == STRING) {
            this->str_value = (string)(*this) + (string)p;
            this->type = STRING;
            return *this;
        } else {
            cerr << "**ERROR** Both values must be same to add." << endl;
            exit(-1);
        }
    }
}

Primitive&
Primitive::operator-(const Primitive& other) {
    if(this->type == other.type) {
        switch(this->type) {
        case INT:
            this->int_val -= other.int_val;
            return *this;
        case FLOAT:
            this->float_val -= other.float_val;
            return *this;
        default:
            cerr << "**ERROR** can't add." << endl;
            exit(-1);
        }
    } else {
        Primitive p = other;
        if(this->type == INT && other.type == FLOAT) {
            this->type = FLOAT;
            this->float_val = p.float_val - (double)this->int_val;
            return *this;
        } else if(this->type == FLOAT && other.type == INT) {
            this->float_val -= (double)other.int_val;
            return *this;
        }
        cerr << "**ERROR** Both values must be same to sub." << endl;
        exit(-1);
    }
}
Primitive& 
Primitive::operator*(const Primitive& other) {
    if(this->type == other.type) {
        switch(this->type) {
        case INT:
            this->int_val *= other.int_val;
            return *this;
        case FLOAT:
            this->float_val *= other.float_val;
            return *this;
        default:
            cerr << "**ERROR** can't add." << endl;
            exit(-1);
        }
    } else {
        Primitive p = other;
        if(this->type == INT && other.type == FLOAT) {
            this->type = FLOAT;
            this->float_val = p.float_val * (double)this->int_val;
            return *this;
        } else if(this->type == FLOAT && other.type == INT) {
            this->float_val *= (double)other.int_val;
            return *this;
        }
        cerr << "**ERROR** Both values must be same to multiply." << endl;
        exit(-1);
    }
}
Primitive& 
Primitive::operator/(const Primitive& other) {
    if(this->type == other.type) {
        switch(this->type) {
        case INT:
            this->int_val /= other.int_val;
            return *this;
        case FLOAT:
            this->float_val /= other.float_val;
            return *this;
        default:
            cerr << "**ERROR** can't add." << endl;
            exit(-1);
        }
    } else {
        Primitive p = other;
        if(this->type == INT && other.type == FLOAT) {
            this->type = FLOAT;
            this->float_val = p.float_val / (double)this->int_val;
            return *this;
        } else if(this->type == FLOAT && other.type == INT) {
            this->float_val /= (double)other.int_val;
            return *this;
        }
        cerr << "**ERROR** Both values must be same to divide." << endl;
        exit(-1);
    }
}

Primitive& 
Primitive::operator%(const Primitive& other) {
    if(this->type == other.type) {
        switch(this->type) {
        case INT:
            this->int_val %= other.int_val;
            return *this;
        default:
            cerr << "**ERROR** can't add." << endl;
            exit(-1);
        }
    } else {
        cerr << "**ERROR** Both values must be same to modulo." << endl;
        exit(-1);
    }
}

Primitive&
Primitive::operator[](int index) {
    if(this->type == ARRAY) {
        if(index < size) {
            return array[index];
        } else {
            fprintf(stderr, "**ERROR** array index out of range (size: %d, index: %d)\n", size, index);
            exit(-1);
        }
    } else {
        fprintf(stderr, "**ERROR** can't get non-array value with index\n");
        exit(-1);
    }
}

Primitive::operator int() {
    if(type == INT) {
        return int_val;
    } else {
        fprintf(stderr, "**ERROR** can't get non-int value\n");
        exit(-1);
    }
}

Primitive::operator double() {
    if(type == FLOAT) {
        return float_val;
    } else {
        fprintf(stderr, "**ERROR** can't get non-float value\n");
        exit(-1);
    }
}

Primitive::operator unsigned char() {
    if(type == INT) {
        return int_val;
    } else {
        fprintf(stderr, "**ERROR** can't get non-float value\n");
        exit(-1);
    }
}

Primitive::operator string() {
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
