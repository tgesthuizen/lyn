#ifndef COMPILER_EXPR_H
#define COMPILER_EXPR_H

#include <cstddef>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "meta.h"

struct expr;
struct type;

enum class constant_type {
  Int,
  Bool,
  Unit,
};

struct constant_expr {
  constant_type type;
  union {
    int i;
    bool b;
  } value;
};

struct variable_expr {
  std::string name;
  int id = 0;
};

struct apply_expr {
  std::unique_ptr<expr> func;
  std::vector<std::unique_ptr<expr>> args;
};

struct lambda_expr {
  std::vector<variable_expr> params;
  std::unique_ptr<expr> body;
};

struct let_binding {
  std::string name;
  int id;
  std::unique_ptr<expr> body;
};

struct let_expr {
  std::vector<let_binding> bindings;
  std::unique_ptr<expr> body;
};

struct begin_expr {
  std::vector<std::unique_ptr<expr>> exprs;
};

struct if_expr {
  std::unique_ptr<expr> cond;
  std::unique_ptr<expr> then;
  std::unique_ptr<expr> els;
};

using all_exprs = type_list<constant_expr, variable_expr, apply_expr,
                            lambda_expr, let_expr, begin_expr, if_expr>;

struct expr {
  derive_pack_t<std::variant, all_exprs> content;
  int line;
  int col;
  struct type *type;

  expr(derive_pack_t<std::variant, all_exprs> content, int line, int col)
    : content{std::move(content)}, line{line}, col{col}, type{nullptr} {}
};

struct toplevel_expr {
  std::string name;
  int id;
  std::unique_ptr<expr> value;
};

std::vector<toplevel_expr> parse(FILE *input);

#endif
