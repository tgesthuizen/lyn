#include "expr.h"
#include "passes.h"
#include "primitives.h"
#include "symbol_table.h"

#include <string_view>
#include <unordered_map>

namespace lyn {

namespace {

void scopify_expr(symbol_table &table, lyn::expr *expr) {
  std::visit(
      [&](auto &&expr) {
        using expr_t = std::decay_t<decltype(expr)>;
        if constexpr (std::is_same_v<expr_t, variable_expr>) {
          expr.id = table.name_to_id.at(expr.name);
        }
        if constexpr (std::is_same_v<expr_t, apply_expr>) {
          scopify_expr(table, expr.func.get());
          for (auto &&arg : expr.args) {
            scopify_expr(table, arg.get());
          }
        }
        if constexpr (std::is_same_v<expr_t, lambda_expr>) {
          std::vector<typename decltype(table.name_to_id)::node_type>
              old_values;
          old_values.reserve(std::size(expr.params));
          for (auto &&param : expr.params) {
            param.id = table.next_id++;
            if (auto handle = table.name_to_id.extract(param.name)) {
              old_values.emplace_back(std::move(handle));
            }
            table.name_to_id[param.name] = param.id;
          }
          scopify_expr(table, expr.body.get());
          for (auto &&param : expr.params)
            table.name_to_id.erase(param.name);
          for (auto &&handle : old_values)
            table.name_to_id.insert(std::move(handle));
        }
        if constexpr (std::is_same_v<expr_t, let_expr>) {
          for (auto &&binding : expr.bindings) {
            scopify_expr(table, binding.body.get());
          }
          std::vector<typename decltype(table.name_to_id)::node_type>
              old_values;
          old_values.reserve(std::size(expr.bindings));
          for (auto &&binding : expr.bindings) {
            binding.id = table.next_id++;
            if (auto &&handle = table.name_to_id.extract(binding.name))
              old_values.emplace_back(std::move(handle));
            table.name_to_id[binding.name] = binding.id;
          }
          scopify_expr(table, expr.body.get());
          for (auto &&binding : expr.bindings)
            table.name_to_id.erase(binding.name);
          for (auto &&handle : old_values)
            table.name_to_id.insert(std::move(handle));
        }
        if constexpr (std::is_same_v<expr_t, begin_expr>) {
          for (auto &&subexpr : expr.exprs) {
            scopify_expr(table, subexpr.get());
          }
        }
        if constexpr (std::is_same_v<expr_t, if_expr>) {
          scopify_expr(table, expr.cond.get());
          scopify_expr(table, expr.then.get());
          scopify_expr(table, expr.els.get());
        }
      },
      expr->content);
}

} // namespace

symbol_table scopify(std::vector<toplevel_expr> &exprs) {
  symbol_table table;
  for (auto &&primitive : primitives) {
    table.register_name(primitive.name);
  }
  table.first_global_id = table.next_id;
  for (auto &&decl : exprs) {
    decl.id = table.register_name(decl.name);
  }
  table.first_local_id = table.next_id;
  for (auto &&decl : exprs) {
    scopify_expr(table, decl.value.get());
  }
  return table;
}

} // namespace lyn
