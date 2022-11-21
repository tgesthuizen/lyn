#ifndef LYN_ANF_H
#define LYN_ANF_H

#include "meta.h"

#include <string>
#include <variant>
#include <vector>

namespace lyn {

struct basic_block;

struct anf_receive {
  std::vector<int> args;
};

struct anf_adjust_stack {};

struct anf_global {
  std::string name;
  int id;
};

struct anf_constant {
  int value;
  int id;
};

struct anf_call {
  int call_id;
  std::vector<int> arg_ids;
  int res_id;
  bool is_tail;
};

struct anf_assoc {
  int alias;
  int id;
};

struct anf_cond {
  int cond_id;
  int then_block;
  int else_block;
};

struct anf_return {
  int value;
};

struct anf_jump {
  int target;
};

struct anf_global_assign {
  std::string name;
  int id;
};

using all_anf_types =
    type_list<anf_receive, anf_adjust_stack, anf_global, anf_constant, anf_call,
              anf_cond, anf_return, anf_assoc, anf_jump, anf_global_assign>;
using anf_expr = derive_pack_t<std::variant, all_anf_types>;

struct basic_block {
  std::vector<anf_expr> content;
};

struct anf_def {
  std::string name;
  std::vector<basic_block> blocks;
  bool global;
};

struct anf_context {
  std::vector<anf_def> defs;
};

} // namespace lyn

#endif
