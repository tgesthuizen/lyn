#include "expr.h"
#include "passes.h"
#include "primitives.h"
#include "symbol_table.h"
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

class typecheck_t {
public:
  explicit typecheck_t(std::pmr::monotonic_buffer_resource &alloc)
      : alloc{alloc} {
    int_t = new (alloc_type()) type{int_type{}};
    bool_t = new (alloc_type()) type{bool_type{}};
    unit_t = new (alloc_type()) type{unit_type{}};
  }

  type *visit(expr &target);

  void setup_primitive_types(const symbol_table &stable);
  void register_typevar(int id) {
    id_to_type[id] = new (alloc_type()) type{type_variable{}};
  }

  type *import_type_expr(const type_expr &expr) {
    return std::visit(
        [this](auto &&type_expr) {
          using type_expr_t = std::decay_t<decltype(type_expr)>;
          if (std::is_same_v<type_expr_t, int_type_expr>)
            return int_t;
          if (std::is_same_v<type_expr_t, bool_type_expr>)
            return bool_t;
          if (std::is_same_v<type_expr_t, unit_type_expr>)
            return unit_t;
          if constexpr (std::is_same_v<type_expr_t, func_type_expr>) {
            assert(!std::empty(type_expr.types));
            std::vector<type *> args;
            std::transform(
                std::begin(type_expr.types), std::end(type_expr.types) - 1,
                std::back_inserter(args),
                [this](auto &&expr) { return import_type_expr(*expr); });
            return new (alloc_type())
                type{function_type{spanify(alloc, args),
                                   import_type_expr(*type_expr.types.back())}};
          }
          unreachable();
        },
        expr.content);
  }

private:
  void *alloc_type() { return alloc.allocate(sizeof(type), alignof(type)); }

  std::pmr::monotonic_buffer_resource &alloc;
  std::unordered_map<int, type *> id_to_type;
  type *int_t;
  type *bool_t;
  type *unit_t;
};

type *typecheck_t::visit(expr &target) {
  const auto typecheck_value = [this](auto &&expr) {
    using expr_t = std::decay_t<decltype(expr)>;
    if constexpr (std::is_same_v<expr_t, constant_expr>) {
      return int_t;
    }
    if constexpr (std::is_same_v<expr_t, variable_expr>) {
      return id_to_type.at(expr.id);
    }
    if constexpr (std::is_same_v<expr_t, apply_expr>) {
      auto *const ftype = visit(*expr.func);
      function_type ft;
      std::vector<type *> params;
      for (auto &&arg : expr.args) {
        params.push_back(visit(*arg));
      }
      ft.params = spanify(alloc, params);
      type *const result = new (alloc_type()) type{type_variable{}};
      ft.result = result;
      if (!unify(new (alloc_type()) type{std::move(ft)}, ftype))
        throw std::runtime_error{"Cannot unify function application"};
      return result;
    }
    if constexpr (std::is_same_v<expr_t, lambda_expr>) {
      std::vector<type *> args;
      for (auto &&param : expr.params) {
        type *const arg = new (alloc_type()) type{type_variable{}};
        id_to_type[param.id] = arg;
        args.push_back(arg);
      }
      auto *const ret = visit(*expr.body);
      return new (alloc_type()) type{function_type{spanify(alloc, args), ret}};
    }
    if constexpr (std::is_same_v<expr_t, let_expr>) {
      for (auto &&binding : expr.bindings) {
        id_to_type[binding.id] = visit(*binding.body);
      }
      if (std::empty(expr.body)) {
        return unit_t;
      }
      std::for_each(std::begin(expr.body), std::end(expr.body) - 1,
                    [this](lyn::expr *ptr) { visit(*ptr); });
      return visit(*expr.body.back());
    }
    if constexpr (std::is_same_v<expr_t, if_expr>) {
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
  };
  return target.type = std::visit(typecheck_value, target.content);
}

void typecheck_t::setup_primitive_types(const symbol_table &stable) {
  // TODO: Is there really no way to create a std::initializer list for a
  // function template call but to bind the brace init list to an auto variable?
  auto bi_int_args = {int_t, int_t};
  type *bi_int = new (alloc_type())
      type{function_type{spanify(alloc, bi_int_args), int_t}};
  auto uni_int_args = {int_t};
  type *uni_int = new (alloc_type())
      type{function_type{spanify(alloc, uni_int_args), int_t}};
  type *comp_int = new (alloc_type())
      type{function_type{spanify(alloc, bi_int_args), bool_t}};
  auto bi_bool_args = {bool_t, bool_t};
  type *bi_bool = new (alloc_type())
      type{function_type{spanify(alloc, bi_bool_args), bool_t}};
  auto uni_bool_args = {bool_t};
  type *uni_bool = new (alloc_type())
      type{function_type{spanify(alloc, uni_bool_args), bool_t}};

  for (auto &&primitive : primitives) {
    id_to_type[stable[primitive.name]] = [&] {
      switch (primitive.type) {
      case primitive_type::int_int_int:
        return bi_int;
      case primitive_type::int_int:
        return uni_int;
      case primitive_type::int_int_bool:
        return comp_int;
      case primitive_type::bool_bool_bool:
        return bi_bool;
      case primitive_type::bool_bool:
        return uni_bool;
      case primitive_type::bool_:
        return bool_t;
      case primitive_type::unit:
        return unit_t;
      }
      throw std::invalid_argument{"Invalid enum value"};
    }();
  }
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
    type *expr_type = nullptr;
    type *type_expr_type = nullptr;
    if (expr.value)
      expr_type = functor.visit(*expr.value);
    if (expr.type_value) {
      type_expr_type = functor.import_type_expr(*expr.type_value);
    }
    if (expr_type && type_expr_type && !unify(expr_type, type_expr_type))
      throw std::runtime_error{"Mismatching decl"};
  }
}

} // namespace lyn
