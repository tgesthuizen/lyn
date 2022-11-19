#include "anf.h"

#include <algorithm>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace lyn {

void genasm(anf_context &ctx, FILE *out) {
  fputs("\t.thumb\n\t.section \".text\", \"ax\"\n", out);
  int label_offset = 1;
  for (auto &&def : ctx.defs) {
    if (def.global)
      fprintf(out, "\t.global \"%s\"\n", def.name.c_str());
    fprintf(out, "\t.type \"%s\", %%function\n\"%s\":\n", def.name.c_str(),
            def.name.c_str());

    std::unordered_map<int, int> local_to_stack_slot;
    std::unordered_map<std::size_t, int> used_stack_slots;
    used_stack_slots[0] = 0;
    for (std::size_t block_idx = 0; block_idx != std::size(def.blocks);
         ++block_idx) {
      auto &&block = def.blocks[block_idx];
      int local_count = used_stack_slots.at(block_idx);
      int stack_offset = local_count;
      fprintf(out, ".L%d:\n", static_cast<int>(label_offset + block_idx));
      for (auto &&expr : block.content)
        std::visit(
            [&](auto &&val) {
              using val_t = std::decay_t<decltype(val)>;
              if constexpr (std::is_same_v<val_t, anf_receive>) {
                local_count +=
                    std::max(std::size(val.args), std::size_t{4u}) + 1;
              }
              if constexpr (std::is_same_v<val_t, anf_global>) {
                local_count += 1;
              }
              if constexpr (std::is_same_v<val_t, anf_constant>) {
                local_count += 1;
              }
              if constexpr (std::is_same_v<val_t, anf_call>) {
                local_count += 1;
              }
            },
            expr);
      for (auto &&expr : block.content) {
        std::visit(
            [&](auto &&val) {
              using val_t = std::decay_t<decltype(val)>;
              if constexpr (std::is_same_v<val_t, anf_receive>) {
                for (std::size_t i = 4; i < std::size(val.args); ++i) {
                  local_to_stack_slot[val.args[i]] = 4 - i;
                }
                for (std::size_t i = 0;
                     i < std::max(std::size(val.args), std::size_t{4u}); ++i) {
                  local_to_stack_slot[val.args[i]] = 5 - i;
                }
                fprintf(out, "\tpush {");
                for (int i = 0; i < static_cast<int>(std::min(
                                        std::size(val.args), std::size_t{4u}));
                     ++i) {
                  fprintf(out, "r%d, ");
                }
                stack_offset +=
                    2 + std::min(std::size(val.args), std::size_t{4u});
                fputs("lr}\n", out);
                fprintf(out, "\tsubs sp, #%d\n", local_count * 4);
              }
              if constexpr (std::is_same_v<val_t, anf_global>) {
                const int stack_slot = stack_offset++;
                local_to_stack_slot[val.id] = stack_slot;
                fprintf(out,
                        "\tldr r0, \"%s\"\n"
                        "\tstr r0, [sp, #%d]\n",
                        val.name.c_str(), stack_slot * 4);
              }
              if constexpr (std::is_same_v<val_t, anf_constant>) {
                const int stack_slot = stack_offset++;
                local_to_stack_slot[val.id] = stack_slot;
                fprintf(out,
                        "\tldr r0, =%d\n"
                        "\tstr r0, [sp, #%d]\n",
                        stack_slot * 4);
              }
              if constexpr (std::is_same_v<val_t, anf_call>) {
                if (std::size(val.arg_ids) > 4)
                  throw std::runtime_error{"Sorry, more than 4 args are WIP"};
                fprintf(out, "\tldr r4, [sp, #%d]\n",
                        local_to_stack_slot[val.call_id] * 4);
                for (std::size_t i = 0; i < std::size(val.arg_ids); ++i) {
                  fprintf(out, "\tldr r%d, [sp, #%d]\n", static_cast<int>(i),
                          local_to_stack_slot[val.arg_ids[i]] * 4);
                }
                if (val.is_tail) {
                  // Unwind the stack frame, restore the old lr.
                  // There might be a faster way to do this...
                  fprintf(out,
                          "\tadd sp, #%d\n"
                          "\tpop {r6}\n"
                          "\tmov r6, lr\n"
                          "\tb r4\n",
                          (local_count - 1) * 4);
                } else {
                  const int stack_slot = stack_offset++;
                  local_to_stack_slot[val.res_id] = stack_slot;
                  fprintf(out,
                          "\tbl r4\n"
                          "\tstr r0, [sp, #%d]\n",
                          stack_slot * 4);
                }
              }
              if constexpr (std::is_same_v<val_t, anf_assoc>) {
                local_to_stack_slot[val.id] = local_to_stack_slot[val.alias];
              }
              if constexpr (std::is_same_v<val_t, anf_cond>) {
                used_stack_slots[val.then_block] = local_count;
                used_stack_slots[val.else_block] = local_count;
                fprintf(out,
                        "\tldr r0, [sp, #%d]\n"
                        "\tbe .L%d\n"
                        "\tb .L%d\n",
                        local_to_stack_slot[val.cond_id],
                        val.then_block + label_offset,
                        val.else_block + label_offset);
              }
              if constexpr (std::is_same_v<val_t, anf_return>) {
                fprintf(out,
                        "\tldr r0, [sp, #%d]\n"
                        "\tadd sp, #%d\n"
                        "\tpop {pc}\n",
                        (local_count - 1) * 4,
                        local_to_stack_slot[val.value] * 4);
              }
              if constexpr (std::is_same_v<val_t, anf_jump>) {
                fprintf(out, "\tb .L%d\n", val.target + label_offset);
              }
              if constexpr (std::is_same_v<val_t, anf_global_assign>) {
                fprintf(out,
                        "\tldr r0, \"%s\"\n"
                        "\tldr r1, [sp, #%d]\n"
                        "\tstr r1, r0\n",
                        val.name.c_str(), local_to_stack_slot[val.id]);
              }
            },
            expr);
      }
    }
    fprintf(out,
            "\t.pool\n"
            "\t.size \"%s\", .-\"%s\"\n",
            def.name.c_str(), def.name.c_str());
  }
}

} // namespace lyn
