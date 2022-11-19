#include "anf.h"

#include <algorithm>
#include <cstdio>

namespace lyn {

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
                if (!std::empty(val.args)) {
                  fprintf(out, "%d", val.args.front());
                  std::for_each(std::begin(val.args) + 1, std::end(val.args),
                                [out](int i) { fprintf(out, ", %d", i); });
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
                fprintf(out, "\t%d <- %scall %d(", val.res_id,
                        val.is_tail ? "tail" : "", val.call_id);
                if (!std::empty(val.arg_ids)) {
                  fprintf(out, "%d", val.arg_ids.front());
                  std::for_each(std::begin(val.arg_ids) + 1,
                                std::end(val.arg_ids),
                                [out](int id) { fprintf(out, ", %d", id); });
                }
                fputs(")\n", out);
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
