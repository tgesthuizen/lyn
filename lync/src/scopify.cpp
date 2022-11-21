#include "expr.h"
#include "passes.h"

#include <string_view>
#include <unordered_map>

namespace lyn {

namespace {

const char *const builtins[]{
    "+", "-",  "*", "/", "%",  "shl", "shr", "lor", "land", "lxor", "neg",
    "=", "!=", "<", ">", "<=", ">=",  "not", "or",  "and",  "xor",
};

class scopify_t {
public:
  explicit scopify_t(symbol_table &table) : table{table} {}

  void operator()([[maybe_unused]] constant_expr &expr) {}
  void operator()(variable_expr &expr) {
    expr.id = table.name_to_id.at(expr.name);
  }
  void operator()(apply_expr &expr) {
    std::visit(*this, expr.func->content);
    for (auto &&arg : expr.args) {
      std::visit(*this, arg->content);
    }
  }
  void operator()(lambda_expr &expr) {
    std::vector<decltype(table.name_to_id)::node_type> old_values;
    old_values.reserve(std::size(expr.params));
    for (auto &&param : expr.params) {
      param.id = table.next_id++;
      if (auto handle = table.name_to_id.extract(param.name)) {
        old_values.emplace_back(std::move(handle));
      }
      table.name_to_id[param.name] = param.id;
    }
    std::visit(*this, expr.body->content);
    for (auto &&param : expr.params)
      table.name_to_id.erase(param.name);
    for (auto &&handle : old_values)
      table.name_to_id.insert(std::move(handle));
  }

  void operator()(let_expr &expr) {
    for (auto &&binding : expr.bindings) {
      std::visit(*this, binding.body->content);
    }
    std::vector<decltype(table.name_to_id)::node_type> old_values;
    old_values.reserve(std::size(expr.bindings));
    for (auto &&binding : expr.bindings) {
      binding.id = table.next_id++;
      if (auto &&handle = table.name_to_id.extract(binding.name))
        old_values.emplace_back(std::move(handle));
      table.name_to_id[binding.name] = binding.id;
    }
    std::visit(*this, expr.body->content);
    for (auto &&binding : expr.bindings)
      table.name_to_id.erase(binding.name);
    for (auto &&handle : old_values)
      table.name_to_id.insert(std::move(handle));
  }

  void operator()(begin_expr &expr) {
    for (auto &&subexpr : expr.exprs) {
      std::visit(*this, subexpr->content);
    }
  }

  void operator()(if_expr &expr) {
    std::visit(*this, expr.cond->content);
    std::visit(*this, expr.then->content);
    std::visit(*this, expr.els->content);
  }

  int register_name(std::string_view name) {
    const int id = table.next_id++;
    table.name_to_id[name] = id;
    return id;
  }

private:
  symbol_table &table;
};

} // namespace

symbol_table scopify(std::vector<toplevel_expr> &exprs) {
  symbol_table table;
  scopify_t functor{table};
  for (auto &&pname : builtins) {
    functor.register_name(pname);
  }
  table.first_global_id = table.next_id;
  for (auto &&decl : exprs) {
    decl.id = functor.register_name(decl.name);
  }
  table.first_local_id = table.next_id;
  for (auto &&decl : exprs) {
    std::visit(functor, decl.value->content);
  }
  return table;
}

} // namespace lyn
