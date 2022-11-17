#ifndef COMPILER_PASSES_H
#define COMPILER_PASSES_H

#include <cstdio>
#include <string_view>
#include <unordered_map>
#include <vector>

struct toplevel_expr;
struct type;

std::vector<toplevel_expr> parse(FILE *input);
void scopify(std::vector<toplevel_expr> &exprs,
             std::unordered_map<std::string_view, int> &primitive_info);
void typecheck(std::vector<toplevel_expr> &exprs,
               std::unordered_map<std::string_view, int> &primitive_info);

#endif
