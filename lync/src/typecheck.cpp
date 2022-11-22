#include "expr.h"
#include "passes.h"
#include "types.h"

#include <algorithm>
#include <stdexcept>
#include <type_traits>
#include <unordered_map>

namespace lyn {

namespace {

bool unify(type *lhs, type *rhs) {
  return std::visit(
      [=](auto &&lhs_val, auto &&rhs_val) {
        using lhs_t = std::decay_t<decltype(lhs_val)>;
        using rhs_t = std::decay_t<decltype(rhs_val)>;
        if constexpr (std::is_same_v<lhs_t, type_variable>) {
          if (!lhs_val.target) {
            lhs_val.target = rhs;
            return true;
          }
          return unify(lhs_val.target, rhs);
        }
        if (std::is_same_v<rhs_t, type_variable>) {
          return unify(rhs, lhs);
        }
        constexpr bool types_match = std::is_same_v<lhs_t, rhs_t>;
        if (!types_match) {
          return false;
        }
        if (std::is_same_v<lhs_t, int_type> ||
            std::is_same_v<lhs_t, bool_type> ||
            std::is_same_v<lhs_t, unit_type>) {
          return true;
        }
        if constexpr (types_match && std::is_same_v<lhs_t, function_type>) {
          return std::equal(std::begin(lhs_val.params),
                            std::end(lhs_val.params),
                            std::begin(rhs_val.params),
                            std::end(rhs_val.params), unify) &&
                 unify(lhs_val.result, rhs_val.result);
        }
      },
      lhs->content, rhs->content);
}

type *unwrap_typevars(type *t) {
  while (true) {
    if (!std::holds_alternative<type_variable>(t->content))
      break;
    auto *const target = std::get<type_variable>(t->content).target;
    if (!target)
      break;
    t = target;
  }
  return t;
}

class typecheck_t {
public:
  explicit typecheck_t(std::pmr::monotonic_buffer_resource &alloc)
      : alloc{alloc} {
    int_t = new (alloc_type()) type{int_type{}};
    bool_t = new (alloc_type()) type{bool_type{}};
    unit_t = new (alloc_type()) type{unit_type{}};
  }

  type *operator()(constant_expr &expr) {
    switch (expr.type) {
    case constant_type::Int:
      return int_t;
    case constant_type::Bool:
      return bool_t;
    case constant_type::Unit:
      return unit_t;
    }
    throw std::invalid_argument{"Reached beyond end of switch"};
  }

  type *operator()(variable_expr &expr) { return id_to_type.at(expr.id); }

  type *operator()(apply_expr &expr) {
    auto *const ftype = visit(*expr.func);
    function_type ft;
    for (auto &&arg : expr.args) {
      ft.params.push_back(visit(*arg));
    }
    type *const result = new (alloc_type()) type{type_variable{}};
    ft.result = result;
    if (!unify(new (alloc_type()) type{std::move(ft)}, ftype))
      throw std::runtime_error{"Cannot unify function application"};
    return result;
  }

  type *operator()(lambda_expr &expr) {
    std::vector<type *> args;
    for (auto &&param : expr.params) {
      type *const arg = new (alloc_type()) type{type_variable{}};
      id_to_type[param.id] = arg;
      args.push_back(arg);
    }
    auto *const ret = visit(*expr.body);
    return new (alloc_type()) type{function_type{std::move(args), ret}};
  }

  type *operator()(let_expr &expr) {
    for (auto &&binding : expr.bindings) {
      id_to_type[binding.id] = visit(*binding.body);
    }
    return visit(*expr.body);
  }

  type *operator()(begin_expr &expr) {
    if (std::empty(expr.exprs)) {
      return unit_t;
    }
    std::for_each(
        std::begin(expr.exprs), std::end(expr.exprs) - 1,
        [this](const std::unique_ptr<lyn::expr> &ptr) { visit(*ptr); });
    return visit(*expr.exprs.back());
  }

  type *operator()(if_expr &expr) {
    auto *const cond_t = visit(*expr.cond);
    if (!unify(bool_t, cond_t)) {
      throw std::runtime_error{"Not a valid condition"};
    }
    auto *const then_t = visit(*expr.then);
    auto *const else_t = visit(*expr.els);
    if (!unify(then_t, else_t)) {
      throw std::runtime_error{"Branches have different type"};
    }
    return then_t;
  }

  type *visit(expr &target) {
    return target.type = std::visit(*this, target.content);
  }

  void setup_primitive_types(const symbol_table &stable);
  void register_typevar(int id) {
    id_to_type[id] = new (alloc_type()) type{type_variable{}};
  }

private:
  void *alloc_type() { return alloc.allocate(sizeof(type), alignof(type)); }

  std::pmr::monotonic_buffer_resource &alloc;
  std::unordered_map<int, type *> id_to_type;
  type *int_t;
  type *bool_t;
  type *unit_t;
};

void typecheck_t::setup_primitive_types(const symbol_table &stable) {
  const auto register_type = [&](std::string_view name, type *t) {
    id_to_type[stable.name_to_id.at(name)] = t;
  };
  type *bi_int = new (alloc_type()) type{function_type{{int_t, int_t}, int_t}};
  register_type("+", bi_int);
  register_type("-", bi_int);
  register_type("*", bi_int);
  register_type("/", bi_int);
  register_type("%", bi_int);
  register_type("shl", bi_int);
  register_type("shr", bi_int);
  register_type("lor", bi_int);
  register_type("land", bi_int);
  register_type("lxor", bi_int);
  type *uni_int = new (alloc_type()) type{function_type{{int_t}, int_t}};
  register_type("neg", uni_int);
  type *comp_int =
      new (alloc_type()) type{function_type{{int_t, int_t}, bool_t}};
  register_type("=", comp_int);
  register_type("!=", comp_int);
  register_type("<", comp_int);
  register_type(">", comp_int);
  register_type("<=", comp_int);
  register_type(">=", comp_int);
  type *uni_bool = new (alloc_type()) type{function_type{{bool_t}, bool_t}};
  register_type("not", uni_bool);
  type *bi_bool =
      new (alloc_type()) type{function_type{{bool_t, bool_t}, bool_t}};
  register_type("or", bi_bool);
  register_type("and", bi_bool);
  register_type("xor", bi_bool);
}

} // namespace

void typecheck(std::vector<toplevel_expr> &exprs, const symbol_table &stable,
               std::pmr::monotonic_buffer_resource &alloc) {
  typecheck_t functor{alloc};
  functor.setup_primitive_types(stable);
  for (auto &expr : exprs) {
    functor.register_typevar(expr.id);
  }
  for (auto &&expr : exprs) {
    functor.visit(*expr.value);
  }
}

} // namespace lyn
