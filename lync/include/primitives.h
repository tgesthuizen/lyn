#ifndef LYN_PRIMTIIVES_H
#define LYN_PRIMITIVES_H

#include <string_view>

namespace lyn {

enum class primitive_type {
  int_int_int,
  int_int,
  int_int_bool,
  bool_bool_bool,
  bool_bool,
  bool_,
  unit,
};

struct primitive_info {
  std::string_view name;
  primitive_type type;
};

inline constexpr int number_of_primitives = 24;
extern const primitive_info primitives[number_of_primitives];

} // namespace lyn

#endif
