#include "anf.h"

#include <fmt/printf.h>

#include <algorithm>
#include <cstdio>

namespace lyn {

namespace {

void print_int_list(const std::vector<int> &lst, FILE *out) {
  if (std::empty(lst))
    return;
  fmt::fprintf(out, "%d", lst.front());
  std::for_each(std::begin(lst) + 1, std::end(lst),
                [out](int i) { fmt::fprintf(out, ", %d", i); });
}

} // namespace

void print_anf(anf_context &ctx, FILE *out) {
  for (auto &&def : ctx.defs) {
    if (def.global)
      fputs("<global> ", out);
    fmt::fprintf(out, "%.*s:\n", static_cast<int>(std::size(def.name)),
            def.name.data());
    for (std::size_t i = 0; i < std::size(def.blocks); ++i) {
      auto &&block = def.blocks[i];
      fmt::fprintf(out, ".L%d:\n", static_cast<int>(i));
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
              if constexpr (std::is_same_v<val_t, anf_adjust_stack>) {
                fputs("\tadjust_stack\n", out);
              }
              if constexpr (std::is_same_v<val_t, anf_global>) {
                fmt::fprintf(out, "\t%d <- global \"%.*s\"\n", val.id,
                        static_cast<int>(std::size(val.name)), val.name.data());
              }
              if constexpr (std::is_same_v<val_t, anf_constant>) {
                fmt::fprintf(out, "\t%d <- const %d\n", val.id, val.value);
              }
              if constexpr (std::is_same_v<val_t, anf_call>) {
                if (val.is_tail) {
                  fputs("\ttailcall ", out);
                  std::visit(
                      [&](auto &&target) {
                        using target_t = std::decay_t<decltype(target)>;
                        if constexpr (std::is_same_v<target_t, int>) {
                          fmt::fprintf(out, "%d(", target);
                        }
                        if constexpr (std::is_same_v<target_t,
                                                     std::string_view>) {
                          fmt::fprintf(out, "\"%.*s\"(",
                                  static_cast<int>(std::size(target)),
                                  std::data(target));
                        }
                      },
                      val.call_target);
                  print_int_list(val.arg_ids, out);
                  fputs(")\n", out);
                } else {
                  fmt::fprintf(out, "\t%d <- call ", val.res_id);
                  std::visit(
                      [&](auto &&target) {
                        using target_t = std::decay_t<decltype(target)>;
                        if constexpr (std::is_same_v<target_t, int>) {
                          fmt::fprintf(out, "%d(", target);
                        }
                        if constexpr (std::is_same_v<target_t,
                                                     std::string_view>) {
                          fmt::fprintf(out, "\"%.*s\"(",
                                  static_cast<int>(std::size(target)),
                                  std::data(target));
                        }
                      },
                      val.call_target);
                  print_int_list(val.arg_ids, out);
                  fputs(")\n", out);
                }
              }
              if constexpr (std::is_same_v<val_t, anf_assoc>) {
                fmt::fprintf(out, "\t%d <- alias %d\n", val.id, val.alias);
              }
              if constexpr (std::is_same_v<val_t, anf_cond>) {
                fmt::fprintf(out, "\tif %d: %d %d\n", val.cond_id, val.then_block,
                        val.else_block);
              }
              if constexpr (std::is_same_v<val_t, anf_global_assign>) {
                fmt::fprintf(out, "\tassign_global \"%.*s\" <- %d\n",
                        static_cast<int>(std::size(val.name)), val.name.data(),
                        val.id);
              }
              if constexpr (std::is_same_v<val_t, anf_return>) {
                fmt::fprintf(out, "\tret %d\n", val.value);
              }
              if constexpr (std::is_same_v<val_t, anf_jump>) {
                fmt::fprintf(out, "\tjmp %d\n", val.target);
              }
            },
            inst);
      }
    }
  }
}

} // namespace lyn
