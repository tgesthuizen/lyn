#ifndef COMPILER_PASSES_H
#define COMPILER_PASSES_H

#include <cstdio>
#include <string_view>
#include <unordered_map>
#include <vector>

struct toplevel_expr;
struct type;

struct symbol_table {
  std::unordered_map<std::string_view, int> name_to_id = {};
  int next_id = 1;
  int first_global_id = 0;
  int first_local_id = 0;
};

std::vector<toplevel_expr> parse(FILE *input);
symbol_table scopify(std::vector<toplevel_expr> &exprs);
void typecheck(std::vector<toplevel_expr> &exprs, const symbol_table &stable);

#endif
