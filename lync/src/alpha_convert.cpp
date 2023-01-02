#include "expr.h"
#include "passes.h"
#include "primitives.h"
#include "symbol_table.h"

#include <algorithm>
#include <cstdio>
#include <string_view>
#include <unordered_map>

namespace lyn {

namespace {

struct alpha_convert_expr {
  bool visit(lyn::expr *expr_ptr);
  symbol_table &table;
};

bool alpha_convert_expr::visit(lyn::expr *expr_ptr) {
  return std::visit(
      [&](auto &&expr) {
        using expr_t = std::decay_t<decltype(expr)>;
        if constexpr (std::is_same_v<expr_t, variable_expr>) {
          expr.id = table[expr.name];
          const bool res = expr.id != 0;
          if (!res) {
            auto &&sloc = expr_ptr->sloc;
            fprintf(stderr, "%.*s:%d:%d: error: No binding \"%.*s\" in scope\n",
                    static_cast<int>(std::size(sloc.file_name)),
                    std::data(sloc.file_name), sloc.line, sloc.col,
                    static_cast<int>(std::size(expr.name)),
                    std::data(expr.name));
          }
          return res;
        }
        if constexpr (std::is_same_v<expr_t, apply_expr>) {
          return visit(expr.func) &&
                 std::all_of(std::begin(expr.args), std::end(expr.args),
                             [this](auto &&arg) { return visit(arg); });
        }
        if constexpr (std::is_same_v<expr_t, lambda_expr>) {
          scope current_scope;
          for (auto &&param : expr.params) {
            param.id = table.register_local(param.name, current_scope);
          }
          const bool result = visit(expr.body) != 0;
          table.pop_scope(current_scope);
          return result;
        }
        if constexpr (std::is_same_v<expr_t, let_expr>) {
          if (!std::all_of(std::begin(expr.bindings), std::end(expr.bindings),
                           [this](auto &&binding) {
                             return visit(binding.body);
                           }))
            return false;
          scope current_scope;
          for (auto &&binding : expr.bindings) {
            binding.id = table.register_local(binding.name, current_scope);
          }
          const bool result =
              std::all_of(std::begin(expr.body), std::end(expr.body),
                          [this](auto &&ptr) { return visit(ptr); });
          table.pop_scope(current_scope);
          return result;
        }
        if constexpr (std::is_same_v<expr_t, if_expr>) {
          return visit(expr.cond) && visit(expr.then) && visit(expr.els);
        }
        return true;
      },
      expr_ptr->content);
}

} // namespace

bool alpha_convert(std::vector<toplevel_expr> &exprs, symbol_table &table) {
  for (auto &&primitive : primitives) {
    table.register_primitive(primitive.name);
  }
  table.start_global_registering();
  for (auto &&decl : exprs) {
    decl.id = table.register_global(decl.name);
  }
  table.start_local_registering();
  alpha_convert_expr ac{table};
  return std::all_of(std::begin(exprs), std::end(exprs), [&](auto &&decl) {
    return !decl.value || ac.visit(decl.value);
  });
}

} // namespace lyn
