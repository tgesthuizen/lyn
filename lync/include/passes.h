#ifndef LYN_PASSES_H
#define LYN_PASSES_H

#include "string_table.h"
#include "symbol_table.h"
#include <cstdio>
#include <memory_resource>
#include <optional>
#include <vector>

namespace lyn {

struct compilation_context {
  string_table stbl;
  symbol_table symtab;
  std::pmr::monotonic_buffer_resource expr_alloc;
  std::pmr::monotonic_buffer_resource type_alloc;
};

struct toplevel_expr;
struct type;
struct anf_context;
struct symbol_table;

std::optional<std::vector<toplevel_expr>>
parse(FILE *f, std::string_view file_name, compilation_context &cc);
bool alpha_convert(std::vector<toplevel_expr> &exprs, symbol_table &table);
void typecheck(std::vector<toplevel_expr> &exprs, const symbol_table &stable,
               std::pmr::monotonic_buffer_resource &alloc);

struct delete_anf {
  void operator()(anf_context *ctx);
};

std::unique_ptr<anf_context, delete_anf>
genanf(std::vector<toplevel_expr> &exprs, string_table &stbl,
       const symbol_table &symtab);
void print_anf(anf_context &ctx, FILE *out);
void genasm(anf_context &ctx, FILE *out);

} // namespace lyn

#endif
