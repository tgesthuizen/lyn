#ifndef COMPILER_TYPES_H
#define COMPILER_TYPES_H

#include <variant>
#include <vector>

#include "meta.h"

struct type;

struct int_type {};
struct bool_type {};
struct unit_type {};

struct function_type {
  std::vector<type *> params;
  type *result;
};

struct type_variable {
  type *target;
};

using all_types =
    type_list<int_type, bool_type, unit_type, function_type, type_variable>;

struct type {
  derive_pack_t<std::variant, all_types> content;
};
#endif
