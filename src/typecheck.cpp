#include "expr.h"
#include "types.h"

#include <algorithm>
#include <stdexcept>
#include <type_traits>
#include <unordered_map>

namespace {

struct unify_t {
  bool operator()(int_type lhs, int_type rhs) { return true; }
  bool operator()(bool_type lhs, bool_type rhs) { return true; }
  bool operator()(unit_type lhs, unit_type rhs) { return true; }

  bool operator()(const function_type &lhs, const function_type &rhs) {
    return std::equal(
               std::begin(lhs.params), std::end(lhs.params),
               std::begin(rhs.params), std::end(rhs.params),
               [this](type *lhs, type *rhs) { return visit(*lhs, *rhs); }) &&
           visit(*lhs.result, *rhs.result);
  }

  bool operator()(const type_variable &lhs, const type_variable &rhs) {
    // This should be caught by visit()
    throw std::runtime_error{"Internal error"};
  }
  template <class T> bool operator()(const type_variable &lhs, const T &other) {
    // This should be caught by visit()
    throw std::runtime_error{"Internal error"};
  }
  template <class T> bool operator()(const T &other, const type_variable &rhs) {
    // This should be caught by visit()
    throw std::runtime_error{"Internal error"};
  }
  template <class A, class B>
  std::enable_if_t<!std::is_same_v<A, B>, bool> operator()(const A &lhs,
                                                           const B &rhs) {
    return false;
  }

  bool visit(type &lhs, type &rhs) {
    if (std::holds_alternative<type_variable>(lhs.content)) {
      auto &&var = std::get<type_variable>(lhs.content);
      if (!var.target) {
        var.target = &rhs;
        return true;
      }
      return visit(*var.target, rhs);
    }
    if (std::holds_alternative<type_variable>(rhs.content))
      return visit(rhs, lhs);
    return std::visit(*this, lhs.content, rhs.content);
  }
};

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

  type *operator()(variable_expr &expr) { return id_to_type[expr.id]; }

  type *operator()(apply_expr &expr) {
    auto *const ftype_boxed = visit(*expr.func);
    auto *ftype = std::get_if<function_type>(&ftype_boxed->content);
    if (!ftype) {
      throw std::runtime_error{"Thing in apply is not a function"};
    }
    if (std::size(ftype->params) != std::size(expr.args)) {
      throw std::runtime_error{"Arity mismatch"};
    }
    if (!std::equal(std::begin(expr.args), std::end(expr.args),
                    std::begin(ftype->params),
                    [this](const std::unique_ptr<::expr> &arg, type *param) {
                      return unify_t{}.visit(*visit(*arg), *param);
                    })) {
      throw std::runtime_error{"Wrong parameter"};
    }
    return ftype->result;
  }

  type *operator()(lambda_expr &expr) {
    std::vector<type *> args;
    for (auto &&param : expr.params) {
      type *const arg = new type{type_variable{}};
      id_to_type[param.id] = arg;
      args.push_back(arg);
    }
    auto *const ret = visit(*expr.body);
    return new type{function_type{std::move(args), ret}};
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
    std::for_each(std::begin(expr.exprs), std::end(expr.exprs) - 1,
                  [this](const std::unique_ptr<::expr> &ptr) { visit(*ptr); });
    return visit(*expr.exprs.back());
  }

  type *operator()(if_expr &expr) {
    auto *const cond_t = visit(*expr.cond);
    if (!unify_t{}.visit(*bool_t, *cond_t)) {
      throw std::runtime_error{"Not a valid condition"};
    }
    auto *const then_t = visit(*expr.then);
    auto *const else_t = visit(*expr.els);
    if (!unify_t{}.visit(*then_t, *else_t)) {
      throw std::runtime_error{"Branches have different type"};
    }
    return then_t;
  }

  type *visit(expr &target) {
    return target.type = std::visit(*this, target.content);
  }

private:
  std::unordered_map<int, type *> id_to_type;
  type *int_t = new type{int_type{}};
  type *bool_t = new type{bool_type{}};
  type *unit_t = new type{unit_type{}};
};

} // namespace

void typecheck(std::vector<toplevel_expr> &exprs) {
  typecheck_t functor;
  for (auto &&expr : exprs) {
    functor.visit(*expr.value);
  }
}
