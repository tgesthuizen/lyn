#include "anf.h"
#include "constants.h"
#include "expr.h"
#include "passes.h"
#include "symbol_table.h"

#include <algorithm>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <variant>

namespace lyn {

namespace {

struct fun_info {
  std::string_view name;
  const lambda_expr &expr;
  bool global;
};

struct local_info {
  int ref_count = 0;
  std::variant<std::monostate, int, std::string_view> rewritable;
};

class anf_generator {
public:
  explicit anf_generator(string_table &stbl, const symbol_table &symtab)
      : stbl{stbl}, symtab{symtab}, next_local_id{symtab.get_next_local_id()},
        next_global_id{symtab.get_next_global_id()} {}

  void push_func(std::string_view name, lambda_expr *ptr) {
    funcs_to_generate.push_back(fun_info{name, *ptr, true});
  }

  void run();
  std::unordered_map<int, local_info> get_local_infos() && {
    return std::move(local_infos);
  }
  anf_context &&get_context() && { return std::move(ctx); }

private:
  int visit_expr(const lyn::expr &value);
  template <class... Args> void emit_instr(Args &&...args) {
    current_block->content.emplace_back(std::forward<Args>(args)...);
  }

  string_table &stbl;
  const symbol_table &symtab;
  int next_local_id;
  int next_global_id;

  std::unordered_map<int, local_info> local_infos;
  std::vector<fun_info> funcs_to_generate = {};
  anf_context ctx = {};
  anf_def *current_def = nullptr;
  basic_block *current_block = nullptr;
  bool tail_pos = true;
};

void anf_generator::run() {
  for (std::size_t i = 0; i < std::size(funcs_to_generate); ++i) {
    auto &&info = funcs_to_generate[i];
    anf_def new_def;
    new_def.name = info.name;
    anf_receive prologue;
    prologue.args.reserve(std::size(info.expr.params));
    std::transform(std::begin(info.expr.params), std::end(info.expr.params),
                   std::back_inserter(prologue.args),
                   [](const variable_expr &expr) { return expr.id; });
    new_def.blocks.emplace_back();
    current_def = &new_def;
    current_block = &new_def.blocks.back();
    emit_instr(std::move(prologue));
    emit_instr(anf_adjust_stack{});
    tail_pos = true;
    visit_expr(*info.expr.body);
    ctx.defs.emplace_back(std::move(new_def));
  }
}

int anf_generator::visit_expr(const lyn::expr &value) {
  const auto visit_fun = [this](auto &&expr) {
    using expr_t = std::decay_t<decltype(expr)>;
    if constexpr (std::is_same_v<expr_t, constant_expr>) {
      const int constant_id = next_local_id++;
      emit_instr(anf_constant{expr.value, constant_id});
      if (tail_pos)
        emit_instr(anf_return{constant_id});
      return constant_id;
    }
    if constexpr (std::is_same_v<expr_t, variable_expr>) {
      if (expr.id >= first_local_id) {
        ++local_infos[expr.id].ref_count;
        if (tail_pos)
          emit_instr(anf_return{expr.id});
        return expr.id;
      }
      const auto global_copy = next_local_id++;
      emit_instr(anf_global{expr.name, global_copy});
      local_infos[global_copy] = {tail_pos ? 1 : 0, expr.name};
      if (tail_pos)
        emit_instr(anf_return{global_copy});
      return global_copy;
    }
    if constexpr (std::is_same_v<expr_t, apply_expr>) {
      const bool tail_pos_saved = std::exchange(tail_pos, false);
      const int fid = visit_expr(*expr.func);
      std::vector<int> args;
      args.reserve(std::size(expr.args));
      std::transform(std::begin(expr.args), std::end(expr.args),
                     std::back_inserter(args), [this](lyn::expr *arg) {
                       const int local_id = visit_expr(*arg);
                       ++local_infos[local_id].ref_count;
                       return local_id;
                     });
      tail_pos = tail_pos_saved;
      int call_id = 0;
      if (!tail_pos)
        call_id = next_local_id++;
      decltype(std::declval<anf_call>().call_target) target = fid;
      if (std::holds_alternative<std::string_view>(
              local_infos[fid].rewritable)) {
        target = std::get<std::string_view>(local_infos[fid].rewritable);
      } else {
        ++local_infos[fid].ref_count;
      }
      emit_instr(anf_call{target, std::move(args), call_id, tail_pos});
      return call_id;
    }
    if constexpr (std::is_same_v<expr_t, lambda_expr>) {
      const int global_id = next_global_id++;
      const auto fun_name = stbl.store("fun" + std::to_string(global_id));
      funcs_to_generate.push_back(fun_info{fun_name, expr, false});
      emit_instr(anf_global{fun_name, global_id});
      return global_id;
    }
    if constexpr (std::is_same_v<expr_t, let_expr>) {
      const auto tail_pos_saved = std::exchange(tail_pos, false);
      for (auto &&binding : expr.bindings) {
        const int bid = visit_expr(*binding.body);
        emit_instr(anf_assoc{bid, binding.id});
      }
      tail_pos = tail_pos_saved;
      if (std::empty(expr.body)) {
        return 0;
      }
      std::for_each(std::begin(expr.body), std::end(expr.body) - 1,
                    [this](lyn::expr *ptr) { visit_expr(*ptr); });
      return visit_expr(*expr.body.back());
    }
    if constexpr (std::is_same_v<expr_t, if_expr>) {
      const auto tail_pos_saved = std::exchange(tail_pos, false);
      const int cond_id = visit_expr(*expr.cond);
      ++local_infos[cond_id].ref_count;
      tail_pos = tail_pos_saved;
      // Please be very aware in the below section that inserting into
      // current_def->blocks might get you a dangling current_block
      const std::size_t this_block_idx =
          current_block - current_def->blocks.data();
      current_def->blocks.emplace_back();
      current_def->blocks.emplace_back();
      int ret_id = -1;
      if (!tail_pos) {
        current_def->blocks.emplace_back();
        ret_id = next_local_id++;
      }
      const int total_blocks = std::size(current_def->blocks);
      const int then_block = total_blocks - 2;
      const int else_block = total_blocks - 1;
      const int cont_block = total_blocks - 3;
      const auto emit_branch = [this, cont_block,
                                ret_id](int block, const lyn::expr &expr) {
        current_block = &current_def->blocks[block];
        emit_instr(anf_adjust_stack{});
        const int expr_id = visit_expr(expr);
        if (tail_pos)
          return;
        emit_instr(anf_assoc{expr_id, ret_id});
        emit_instr(anf_jump{cont_block});
      };
      current_block = &current_def->blocks[this_block_idx];
      emit_instr(anf_cond{cond_id, then_block, else_block});
      emit_branch(then_block, *expr.then);
      emit_branch(else_block, *expr.els);
      if (tail_pos)
        return 0;
      current_block = &current_def->blocks[cont_block];
      return ret_id;
    }
  };
  return std::visit(visit_fun, value.content);
}

struct anf_dead_code_elim {
  bool can_be_deleted(const anf_expr &expr);
  void run();

  std::unordered_map<int, local_info> local_infos;
  anf_context ctx;
};

bool anf_dead_code_elim::can_be_deleted(const anf_expr &expr) {
  return std::visit(
      [this](auto &&expr) {
        using expr_t = std::decay_t<decltype(expr)>;
        if (std::is_same_v<expr_t, anf_receive>)
          // Receives cannot be deleted, they are required for stack management
          return false;
        if (std::is_same_v<expr_t, anf_adjust_stack>)
          return false;
        if constexpr (std::is_same_v<expr_t, anf_global>)
          return local_infos[expr.id].ref_count == 0;
        if constexpr (std::is_same_v<expr_t, anf_constant>)
          return local_infos[expr.id].ref_count == 0;
        if (std::is_same_v<expr_t, anf_call>)
          // TODO: We could inspect the call for being side-effect free here
          return false;
        if constexpr (std::is_same_v<expr_t, anf_assoc>)
          return local_infos[expr.id].ref_count == 0;
        if (std::is_same_v<expr_t, anf_cond>)
          return false;
        if (std::is_same_v<expr_t, anf_return>)
          return false;
        if (std::is_same_v<expr_t, anf_jump>)
          return false;
        if (std::is_same_v<expr_t, anf_global_assign>)
          return false;
      },
      expr);
}

void anf_dead_code_elim::run() {
  for (auto &&def : ctx.defs) {
    for (auto &&block : def.blocks) {
      const auto block_end = std::end(block.content);
      block.content.erase(std::remove_if(std::begin(block.content), block_end,
                                         [this](const anf_expr &expr) {
                                           return can_be_deleted(expr);
                                         }),
                          block_end);
    }
  }
}

} // namespace

std::unique_ptr<anf_context, delete_anf>
genanf(std::vector<toplevel_expr> &exprs, string_table &stbl,
       const symbol_table &symtab) {
  anf_generator gen(stbl, symtab);
  for (auto &&expr : exprs) {
    if (!expr.value)
      continue;
    if (!std::holds_alternative<lambda_expr>(expr.value->content)) {
      fprintf(stderr, "Will not generate anything for %.*s!\n",
              static_cast<int>(std::size(expr.name)), expr.name.data());
      continue;
    }
    gen.push_func(expr.name, &std::get<lambda_expr>(expr.value->content));
  }
  gen.run();
  // anf_dead_code_elim eliminator{std::move(gen).get_local_infos(),
  //                               std::move(gen).get_context()};
  // eliminator.run();

  // return std::unique_ptr<anf_context, delete_anf>{
  //     new anf_context{std::move(eliminator.ctx)}};
  return std::unique_ptr<anf_context, delete_anf>{
      new anf_context{std::move(gen).get_context()}};
}

void delete_anf::operator()(anf_context *ctx) { delete ctx; }

} // namespace lyn
