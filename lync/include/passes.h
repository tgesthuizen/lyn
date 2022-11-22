#ifndef LYN_PASSES_H
#define LYN_PASSES_H

#include <cstdio>
#include <memory_resource>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace lyn {

struct toplevel_expr;
struct type;
struct anf_context;
struct symbol_table;

std::vector<toplevel_expr> parse(FILE *input);
symbol_table alpha_convert(std::vector<toplevel_expr> &exprs);
void typecheck(std::vector<toplevel_expr> &exprs, const symbol_table &stable,
               std::pmr::monotonic_buffer_resource &alloc);

struct delete_anf {
  void operator()(anf_context *ctx);
};

std::unique_ptr<anf_context, delete_anf>
genanf(std::vector<toplevel_expr> &exprs, const symbol_table &stable);
void print_anf(anf_context &ctx, FILE *out);
void genasm(anf_context &ctx, FILE *out);

} // namespace lyn

#endif
