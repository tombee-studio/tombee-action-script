#include "../include/primitive.hpp"

int
main() {
    const char* test_id = "test";
    Primitive p1 = Primitive::make_int(0);
    ASSERT((int)p1 == 0);

    Primitive p2 = Primitive::make_float(0);
    ASSERT((double)p2 == 0);

    Primitive p3 = Primitive::make_int(1);
    Primitive p4 = Primitive::make_int(2);
    ASSERT((int)(p3 + p4) == 3);

    Primitive p5 = Primitive::make_float(1.);
    Primitive p6 = Primitive::make_float(2.);
    ASSERT((double)(p5 + p6) == 3.);

    Primitive p7 = Primitive::make_id(test_id);
    ASSERT((string)p7 == test_id);

    return 0;
}