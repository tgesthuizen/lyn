#include "expr.h"
#include "passes.h"
#include "primitives.h"
#include "symbol_table.h"

#include <string_view>
#include <unordered_map>

namespace lyn {

namespace {

void alpha_convert_expr(symbol_table &table, lyn::expr *expr) {
  std::visit(
      [&](auto &&expr) {
        using expr_t = std::decay_t<decltype(expr)>;
        if constexpr (std::is_same_v<expr_t, variable_expr>) {
          expr.id = table[expr.name];
        }
        if constexpr (std::is_same_v<expr_t, apply_expr>) {
          alpha_convert_expr(table, expr.func);
          for (auto &&arg : expr.args) {
            alpha_convert_expr(table, arg);
          }
        }
        if constexpr (std::is_same_v<expr_t, lambda_expr>) {
          scope current_scope;
          for (auto &&param : expr.params) {
            param.id = table.register_local(param.name, current_scope);
          }
          alpha_convert_expr(table, expr.body);
          table.pop_scope(current_scope);
        }
        if constexpr (std::is_same_v<expr_t, let_expr>) {
          for (auto &&binding : expr.bindings) {
            alpha_convert_expr(table, binding.body);
          }
          scope current_scope;
          for (auto &&binding : expr.bindings) {
            binding.id = table.register_local(binding.name, current_scope);
          }
          for (auto &&ptr : expr.body) {
            alpha_convert_expr(table, ptr);
          }
          table.pop_scope(current_scope);
        }
        if constexpr (std::is_same_v<expr_t, if_expr>) {
          alpha_convert_expr(table, expr.cond);
          alpha_convert_expr(table, expr.then);
          alpha_convert_expr(table, expr.els);
        }
      },
      expr->content);
}

} // namespace

symbol_table alpha_convert(std::vector<toplevel_expr> &exprs) {
  symbol_table table;
  for (auto &&primitive : primitives) {
    table.register_primitive(primitive.name);
  }
  table.start_global_registering();
  for (auto &&decl : exprs) {
    decl.id = table.register_global(decl.name);
  }
  table.start_local_registering();
  for (auto &&decl : exprs) {
    alpha_convert_expr(table, decl.value);
  }
  return table;
}

} // namespace lyn
