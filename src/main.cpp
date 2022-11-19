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
  auto decls = lyn::parse(input);
  auto table = lyn::scopify(decls);
  lyn::typecheck(decls, table);
  const auto anfctx = lyn::genanf(decls, table);
}
