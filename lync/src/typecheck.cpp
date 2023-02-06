#include "expr.h"
#include "passes.h"
#include "primitives.h"
#include "symbol_table.h"
#include "types.h"

#include <algorithm>
#include <cstdio>
#include <stdexcept>
#include <type_traits>
#include <unordered_map>

namespace lyn {

namespace {

void print_type(type *lhs);
void print_type(int_type) { fputs("int", stderr); }
void print_type(bool_type) { fputs("bool", stderr); }
void print_type(unit_type) { fputs("unit", stderr); }
void print_type(const function_type &type) {

  fputs("(-> ", stderr);
  for (auto &&param : type.params) {
    print_type(param);
    fputc(' ', stderr);
  }
  print_type(type.result);
  fputc(')', stderr);
}
void print_type(const type_variable &var) {
  if (!var.target) {
    fputs("[ ]", stderr);
  } else {
    print_type(var.target);
  }
}
void print_type(type *lhs) {
  std::visit([](auto &&expr) { print_type(expr); }, lhs->content);
}

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
  void register_type(int id, type *t) { id_to_type[id] = t; }

  type *get_type_for_id(int id) { return id_to_type.at(id); }

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
  const auto typecheck_value = [this, &target](auto &&expr) -> type * {
    using expr_t = std::decay_t<decltype(expr)>;
    if constexpr (std::is_same_v<expr_t, constant_expr>) {
      return int_t;
    }
    if constexpr (std::is_same_v<expr_t, variable_expr>) {
      return id_to_type.at(expr.id);
    }
    if constexpr (std::is_same_v<expr_t, apply_expr>) {
      auto *const ftype = visit(*expr.func);
      if (!ftype)
        return nullptr;
      function_type ft;
      std::vector<type *> params;
      for (auto &&arg : expr.args) {
        const auto arg_t = visit(*arg);
        if (!arg_t)
          return nullptr;
        params.push_back(arg_t);
      }
      ft.params = spanify(alloc, params);
      type *const result = new (alloc_type()) type{type_variable{}};
      ft.result = result;
      if (auto *const applied_type = new (alloc_type()) type{std::move(ft)};
          !unify(applied_type, ftype)) {
        fprintf(stderr, "%.*s:%d:%d: error: applying function of type ",
                static_cast<int>(std::size(target.sloc.file_name)),
                std::data(target.sloc.file_name), target.sloc.line,
                target.sloc.col);
        print_type(ftype);
        fputs(" where ", stderr);
        print_type(applied_type);
        fputs(" is expected\n", stderr);
        return nullptr;
      }
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
      if (!ret)
        return nullptr;
      return new (alloc_type()) type{function_type{spanify(alloc, args), ret}};
    }
    if constexpr (std::is_same_v<expr_t, let_expr>) {
      for (auto &&binding : expr.bindings) {
        id_to_type[binding.id] = visit(*binding.body);
      }
      if (std::empty(expr.body)) {
        return unit_t;
      }
      if (!std::all_of(
              std::begin(expr.body), std::end(expr.body) - 1,
              [this](lyn::expr *ptr) { return visit(*ptr) != nullptr; }))
        return nullptr;
      return visit(*expr.body.back());
    }
    if constexpr (std::is_same_v<expr_t, if_expr>) {
      auto *const cond_t = visit(*expr.cond);
      if (!cond_t)
        return nullptr;
      if (!unify(bool_t, cond_t)) {
        fprintf(stderr, "%.*s:%d:%d: error: Using expression of type ",
                static_cast<int>(std::size(target.sloc.file_name)),
                std::data(target.sloc.file_name), target.sloc.line,
                target.sloc.col);
        print_type(cond_t);
        fputs(" in if condition\n", stderr);
        return nullptr;
      }
      auto *const then_t = visit(*expr.then);
      if (!then_t)
        return nullptr;
      auto *const else_t = visit(*expr.els);
      if (!else_t)
        return nullptr;
      if (!unify(then_t, else_t)) {
        fprintf(stderr, "%.*s:%d:%d: error: if branches do not unify\n",
                static_cast<int>(std::size(target.sloc.file_name)),
                std::data(target.sloc.file_name), target.sloc.line,
                target.sloc.col);
        fprintf(stderr, "%.*s:%d:%d: info: then branch of type ",
                static_cast<int>(std::size(expr.then->sloc.file_name)),
                std::data(expr.then->sloc.file_name), expr.then->sloc.line,
                expr.then->sloc.col);
        print_type(then_t);
        fputc('\n', stderr);
        fprintf(stderr, "%.*s:%d:%d: info: else branch of type ",
                static_cast<int>(std::size(expr.els->sloc.file_name)),
                std::data(expr.els->sloc.file_name), expr.els->sloc.line,
                expr.els->sloc.col);
        print_type(else_t);
        fputc('\n', stderr);
        return nullptr;
      }
      return then_t;
    }
  };
  return target.type = std::visit(typecheck_value, target.content);
}

void typecheck_t::setup_primitive_types(const symbol_table &symtab) {
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
    id_to_type[symtab[primitive.name]] = [&] {
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
      unreachable();
    }();
  }
}

} // namespace

bool typecheck(std::vector<toplevel_expr> &exprs, const symbol_table &symtab,
               std::pmr::monotonic_buffer_resource &alloc) {
  typecheck_t functor{alloc};
  functor.setup_primitive_types(symtab);
  for (auto &expr : exprs) {
    if (expr.type_value)
      functor.register_type(expr.id,
                            functor.import_type_expr(*expr.type_value));
    else
      functor.register_typevar(expr.id);
  }
  for (auto &&expr : exprs) {
    if (!expr.value)
      continue;
    type *const expr_type = functor.visit(*expr.value);
    if (!expr_type)
      return false;
    type *const decl_type = functor.get_type_for_id(expr.id);
    if (!unify(expr_type, decl_type)) {
      fprintf(stderr,
              "%.*s:%d:%d: error: Function definition \"%.*s\" is of "
              "unexpected type:\n"
              "info: Definition is of type: ",
              static_cast<int>(std::size(expr.value->sloc.file_name)),
              std::data(expr.value->sloc.file_name), expr.value->sloc.line,
              expr.value->sloc.col, static_cast<int>(std::size(expr.name)),
              std::data(expr.name));
      print_type(expr_type);
      fprintf(stderr, "\ninfo: Expected type: ");
      print_type(decl_type);
      fputc('\n', stderr);
      return false;
    }
  }
  return true;
}

} // namespace lyn
