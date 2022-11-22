#include "anf.h"
#include "expr.h"
#include "passes.h"
#include "symbol_table.h"

#include <algorithm>
#include <utility>

namespace lyn {

namespace {

struct fun_info {
  std::string name;
  lambda_expr &expr;
  bool global;
};

class anf_generator {
public:
  explicit anf_generator(const symbol_table &stable)
      : stable{stable}, next_id{stable.get_next_id()} {}

  void push_func(std::string name, lambda_expr *ptr) {
    funcs_to_generate.push_back(fun_info{std::move(name), *ptr, true});
  }

  void run() {
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
      new_def.blocks.back().content.emplace_back(std::move(prologue));
      new_def.blocks.back().content.emplace_back(anf_adjust_stack{});
      current_def = &new_def;
      current_block = &new_def.blocks.back();
      tail_pos = true;
      std::visit(*this, info.expr.body->content);
      ctx.defs.emplace_back(std::move(new_def));
    }
  }

  int operator()(constant_expr &expr) {
    const int constant_id = next_id++;
    switch (expr.type) {
    case constant_type::Int:
      current_block->content.emplace_back(
          anf_expr{anf_constant{expr.value.i, constant_id}});
      break;
    case constant_type::Bool:
      current_block->content.emplace_back(
          anf_expr{anf_constant{static_cast<int>(expr.value.b), constant_id}});
      break;
    case constant_type::Unit:
      current_block->content.emplace_back(
          anf_expr{anf_constant{0, constant_id}});
      break;
    }
    if (tail_pos)
      current_block->content.emplace_back(anf_expr{anf_return{constant_id}});
    return constant_id;
  }

  int operator()(variable_expr &expr) {
    if (expr.id >= stable.get_first_local_id()) {
      if (tail_pos)
        current_block->content.emplace_back(anf_return{expr.id});
      return expr.id;
    }
    const auto global_id = next_id++;
    current_block->content.emplace_back(anf_global{expr.name, global_id});
    return global_id;
  }

  int operator()(apply_expr &expr) {
    const bool tail_pos_saved = std::exchange(tail_pos, false);
    const int fid = std::visit(*this, expr.func->content);
    std::vector<int> args;
    args.reserve(std::size(expr.args));
    std::transform(std::begin(expr.args), std::end(expr.args),
                   std::back_inserter(args),
                   [this](const std::unique_ptr<lyn::expr> &arg) {
                     return std::visit(*this, arg->content);
                   });
    tail_pos = tail_pos_saved;
    int call_id = 0;
    if (!tail_pos)
      call_id = next_id++;
    current_block->content.emplace_back(
        anf_call{fid, std::move(args), call_id, tail_pos});
    return call_id;
  }

  int operator()(lambda_expr &expr) {
    const int lambda_id = next_id++;
    std::string fun_name = "fun" + std::to_string(lambda_id);
    funcs_to_generate.push_back(fun_info{fun_name, expr, false});
    current_block->content.emplace_back(anf_global{fun_name, lambda_id});
    return lambda_id;
  }

  int operator()(let_expr &expr) {
    const auto tail_pos_saved = std::exchange(tail_pos, false);
    for (auto &&binding : expr.bindings) {
      const int bid = std::visit(*this, binding.body->content);
      current_block->content.emplace_back(anf_assoc{bid, binding.id});
    }
    tail_pos = tail_pos_saved;
    return std::visit(*this, expr.body->content);
  }

  int operator()(begin_expr &expr) {
    std::for_each(std::begin(expr.exprs), std::end(expr.exprs) - 1,
                  [this](const std::unique_ptr<lyn::expr> &ptr) {
                    return std::visit(*this, ptr->content);
                  });
    return std::visit(*this, expr.exprs.back()->content);
  }

  int operator()(if_expr &expr) {
    const auto tail_pos_saved = std::exchange(tail_pos, false);
    const int cond_id = std::visit(*this, expr.cond->content);
    tail_pos = tail_pos_saved;
    // Please be very aware in the below section that inserting into
    // current_def->blocks might get you a dangling current_block
    const std::size_t this_block_idx =
        current_block - current_def->blocks.data();
    current_def->blocks.emplace_back();
    current_def->blocks.emplace_back();
    if (!tail_pos)
      current_def->blocks.emplace_back();
    const int total_blocks = std::size(current_def->blocks);
    const int then_block = total_blocks - 2;
    const int else_block = total_blocks - 1;
    const int cont_block = total_blocks - 3;
    current_def->blocks[this_block_idx].content.push_back(
        anf_cond{cond_id, then_block, else_block});
    current_block = &current_def->blocks[then_block];
    current_block->content.emplace_back(anf_adjust_stack{});
    const auto then_id = std::visit(*this, expr.then->content);
    current_block = &current_def->blocks[else_block];
    current_block->content.emplace_back(anf_adjust_stack{});
    const auto else_id = std::visit(*this, expr.els->content);
    if (tail_pos)
      return 0;
    const int ret_id = next_id++;
    current_def->blocks[then_block].content.emplace_back(
        anf_assoc{then_block, ret_id});
    current_def->blocks[then_block].content.emplace_back(anf_jump{cont_block});
    current_def->blocks[else_block].content.emplace_back(
        anf_assoc{then_block, ret_id});
    current_def->blocks[else_block].content.emplace_back(anf_jump{cont_block});
    current_block = &current_def->blocks[cont_block];
    return ret_id;
  }

  int get_next_id() const { return next_id; }
  anf_context &&get_context() && { return std::move(ctx); }

private:
  std::unordered_map<int, int> local_mapping;
  const symbol_table &stable;
  std::vector<fun_info> funcs_to_generate;
  anf_context ctx;
  anf_def *current_def;
  basic_block *current_block;
  int next_id;
  bool tail_pos;
};

} // namespace

std::unique_ptr<anf_context, delete_anf>
genanf(std::vector<toplevel_expr> &exprs, const symbol_table &stable) {
  anf_generator gen(stable);
  for (auto &&expr : exprs) {
    if (!std::holds_alternative<lambda_expr>(expr.value->content)) {
      fprintf(stderr, "Will not generate anything for %s!\n",
              expr.name.c_str());
      continue;
    }
    gen.push_func(expr.name, &std::get<lambda_expr>(expr.value->content));
  }
  gen.run();
  return std::unique_ptr<anf_context, delete_anf>{
      new anf_context{std::move(gen).get_context()}};
}

void delete_anf::operator()(anf_context *ctx) { delete ctx; }

} // namespace lyn
