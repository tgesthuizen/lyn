#include "expr.h"
#include "passes.h"

#include <string_view>
#include <unordered_map>

namespace {

const char *const builtins[]{
    "+", "-",  "*", "/", "%",  "shl", "shr", "lor", "land", "lxor", "neg",
    "=", "!=", "<", ">", "<=", ">=",  "not", "or",  "and",  "xor",
};

class scopify_t {
public:
  void operator()([[maybe_unused]] constant_expr &expr) {}
  void operator()(variable_expr &expr) { expr.id = name_to_id.at(expr.name); }
  void operator()(apply_expr &expr) {
    std::visit(*this, expr.func->content);
    for (auto &&arg : expr.args) {
      std::visit(*this, arg->content);
    }
  }
  void operator()(lambda_expr &expr) {
    std::vector<decltype(name_to_id)::node_type> old_values;
    old_values.reserve(std::size(expr.params));
    for (auto &&param : expr.params) {
      param.id = next_id++;
      if (auto handle = name_to_id.extract(param.name)) {
        old_values.emplace_back(std::move(handle));
      }
      name_to_id[param.name] = param.id;
    }
    std::visit(*this, expr.body->content);
    for (auto &&param : expr.params)
      name_to_id.erase(param.name);
    for (auto &&handle : old_values)
      name_to_id.insert(std::move(handle));
  }

  void operator()(let_expr &expr) {
    for (auto &&binding : expr.bindings) {
      std::visit(*this, binding.body->content);
    }
    std::vector<decltype(name_to_id)::node_type> old_values;
    old_values.reserve(std::size(expr.bindings));
    for (auto &&binding : expr.bindings) {
      binding.id = next_id++;
      if (auto &&handle = name_to_id.extract(binding.name))
        old_values.emplace_back(std::move(handle));
      name_to_id[binding.name] = binding.id;
    }
    std::visit(*this, expr.body->content);
    for (auto &&binding : expr.bindings)
      name_to_id.erase(binding.name);
    for (auto &&handle : old_values)
      name_to_id.insert(std::move(handle));
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
    const int id = next_id++;
    name_to_id[name] = id;
    return id;
  }

private:
  std::unordered_map<std::string_view, int> name_to_id;
  int next_id = 1;
};

} // namespace

void scopify(std::vector<toplevel_expr> &exprs,
             std::unordered_map<std::string_view, int> &pinfo) {
  scopify_t functor;
  for (auto &&pname : builtins) {
    pinfo[pname] = functor.register_name(pname);
  }

  for (auto &&decl : exprs) {
    decl.id = functor.register_name(decl.name);
  }

  for (auto &&decl : exprs) {
    std::visit(functor, decl.value->content);
  }
}
