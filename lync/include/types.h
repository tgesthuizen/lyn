#ifndef LYN_TYPES_H
#define LYN_TYPES_H

#include "meta.h"
#include "span.h"
#include <variant>
#include <vector>

namespace lyn {

struct type;

struct int_type {};
struct bool_type {};
struct unit_type {};

struct function_type {
  span<type *> params;
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

} // namespace lyn

#endif
