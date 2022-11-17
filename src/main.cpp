#include <cstdio>

#include "expr.h"
#include "passes.h"

int main(int argc, char **argv) {
  if (argc != 2) {
    fputs("Usage: compiler <input-file>\n", stderr);
    return 1;
  }
  FILE *input = fopen(argv[1], "r");
  if (!input) {
    fputs("Could not open input file\n", stderr);
    return 1;
  }
  auto decls = parse(input);
  std::unordered_map<std::string_view, int> pinfo;
  scopify(decls, pinfo);
  typecheck(decls, pinfo);
}
