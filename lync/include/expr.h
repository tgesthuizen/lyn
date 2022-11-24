#ifndef LYN_EXPR_H
#define LYN_EXPR_H

#include "meta.h"
#include "span.h"
#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace lyn {

struct source_location {
  std::string_view file_name;
  int line;
  int col;
};

struct expr;
struct type;

enum class constant_type {
  Int,
  Bool,
  Unit,
};

struct constant_expr {
  int value;
};

struct variable_expr {
  std::string_view name;
  int id = 0;
};

struct apply_expr {
  expr *func;
  span<expr *> args;
};

struct lambda_expr {
  span<variable_expr> params;
  expr *body;
};

struct let_binding {
  std::string_view name;
  int id;
  expr *body;
};

struct let_expr {
  span<let_binding> bindings;
  span<expr *> body;
};

struct if_expr {
  expr *cond;
  expr *then;
  expr *els;
};

using all_exprs = type_list<constant_expr, variable_expr, apply_expr,
                            lambda_expr, let_expr, if_expr>;

struct expr {
  derive_pack_t<std::variant, all_exprs> content;
  source_location sloc;
  struct type *type;
};

struct type_expr;

struct int_type_expr {};
struct bool_type_expr {};
struct unit_type_expr {};
struct func_type_expr {
  std::vector<type_expr> types;
};
using all_type_exprs =
    type_list<int_type_expr, bool_type_expr, unit_type_expr, func_type_expr>;

struct type_expr {
  derive_pack_t<std::variant, all_type_exprs> content;
};

struct toplevel_expr {
  std::string_view name;
  int id;
  expr *value;
};

} // namespace lyn

#endif
