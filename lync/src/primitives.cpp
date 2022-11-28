#include "primitives.h"

namespace lyn {

const primitive_info primitives[number_of_primitives] = {
    {"+", primitive_type::int_int_int},
    {"-", primitive_type::int_int_int},
    {"*", primitive_type::int_int_int},
    {"/", primitive_type::int_int_int},
    {"%", primitive_type::int_int_int},
    {"shl", primitive_type::int_int_int},
    {"shr", primitive_type::int_int_int},
    {"lor", primitive_type::int_int_int},
    {"land", primitive_type::int_int_int},
    {"lxor", primitive_type::int_int_int},
    {"neg", primitive_type::int_int},
    {"=", primitive_type::int_int_bool},
    {"!=", primitive_type::int_int_bool},
    {"<", primitive_type::int_int_bool},
    {">", primitive_type::int_int_bool},
    {"<=", primitive_type::int_int_bool},
    {">=", primitive_type::int_int_bool},
    {"not", primitive_type::bool_bool},
    {"or", primitive_type::bool_bool_bool},
    {"and", primitive_type::bool_bool_bool},
    {"xor", primitive_type::bool_bool_bool},
    {"true", primitive_type::bool_},
    {"false", primitive_type::bool_},
    {"<>", primitive_type::unit},
};

}
