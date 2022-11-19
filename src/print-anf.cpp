#include "anf.h"

#include <algorithm>
#include <cstdio>

namespace lyn {

namespace {

void print_int_list(const std::vector<int> &lst, FILE *out) {
  if (std::empty(lst))
    return;
  fprintf(out, "%d", lst.front());
  std::for_each(std::begin(lst) + 1, std::end(lst),
                [out](int i) { fprintf(out, ", %d", i); });
}

} // namespace

void print_anf(anf_context &ctx, FILE *out) {
  for (auto &&def : ctx.defs) {
    if (def.global)
      fputs("<global> ", out);
    fprintf(out, "%s:\n", def.name.c_str());
    for (std::size_t i = 0; i < std::size(def.blocks); ++i) {
      auto &&block = def.blocks[i];
      fprintf(out, ".L%d:\n", static_cast<int>(i));
      for (auto &&inst : block.content) {
        std::visit(
            [out](auto &&val) {
              using val_t = std::decay_t<decltype(val)>;
              if constexpr (std::is_same_v<val_t, anf_receive>) {
                fputs("\t", out);
                print_int_list(val.args, out);
                if (!std::empty(val.args)) {
                  fputs(" <- ", out);
                }
                fputs("receive\n", out);
              }
              if constexpr (std::is_same_v<val_t, anf_global>) {
                fprintf(out, "\t%d <- global \"%s\"\n", val.id,
                        val.name.c_str());
              }
              if constexpr (std::is_same_v<val_t, anf_constant>) {
                fprintf(out, "\t%d <- const %d\n", val.id, val.value);
              }
              if constexpr (std::is_same_v<val_t, anf_call>) {
                if (val.is_tail) {
                  fprintf(out, "\ttailcall %d(", val.call_id);
                  print_int_list(val.arg_ids, out);
                  fputs(")\n", out);
                } else {
                  fprintf(out, "\t%d <- call %d(", val.res_id, val.call_id);
                  print_int_list(val.arg_ids, out);
                  fputs(")\n", out);
                }
              }
              if constexpr (std::is_same_v<val_t, anf_assoc>) {
                fprintf(out, "\t%d <- alias %d\n", val.id, val.alias);
              }
              if constexpr (std::is_same_v<val_t, anf_cond>) {
                fprintf(out, "\tif %d: %d %d\n", val.cond_id, val.then_block,
                        val.else_block);
              }
              if constexpr (std::is_same_v<val_t, anf_global_assign>) {
                fprintf(out, "\tassign_global \"%s\" <- %d\n", val.name.c_str(),
                        val.id);
              }
              if constexpr (std::is_same_v<val_t, anf_return>) {
                fprintf(out, "\tret %d\n", val.value);
              }
              if constexpr (std::is_same_v<val_t, anf_jump>) {
                fprintf(out, "\tjmp %d\n", val.target);
              }
            },
            inst);
      }
    }
  }
}

} // namespace lyn
