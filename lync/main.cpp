#include "expr.h"
#include "passes.h"
#include "string_table.h"
#include "symbol_table.h"
#include <cstdio>
#include <stdexcept>
#include <unistd.h>

namespace {

const char help_text[] =
    "Usage: lync [options] <input-file>\n"
    " -o <file>\tSpecifies the output file\n"
    " -d\tDumps the intermediate format instead of generating code\n"
    " -s\tSimply performs a syntax check and exits\n"
    " -h\tPrints this message\n";

std::unique_ptr<lyn::anf_context, lyn::delete_anf>
exec_frontend(FILE *input, std::string_view file_name,
              lyn::compilation_context &cc) {
  auto decls = lyn::parse(input, file_name, cc);
  if (!decls)
    return nullptr;
  auto table = lyn::alpha_convert(*decls);
  lyn::typecheck(*decls, table, cc.type_alloc);
  return lyn::genanf(*decls, table);
}

} // namespace

int main(int argc, char **argv) try {
  enum mode_t {
    stop,
    syntax_only,
    dump_ir,
    full_compile,
  } mode = full_compile;
  int code = 0;
  std::string_view input_name;
  FILE *input = nullptr;
  FILE *target = stdout;
  int ret;
  while (ret = getopt(argc, argv, "ho:ds"), ret != -1 && mode != stop) {
    switch (ret) {
    case 'o':
      if (std::string_view("-") == optarg) {
        target = stdout;
      } else {
        target = fopen(optarg, "w");
        if (!target) {
          fprintf(stderr, "Could not open output file \"%s\"\n", optarg);
          mode = stop;
          code = 1;
        }
      }
      break;
    case 'd':
      mode = dump_ir;
      break;
    case 's':
      mode = syntax_only;
      break;
    case 'h':
      fputs(help_text, stdout);
      mode = stop;
      break;
    default:
      fprintf(stderr, "Unknown option -%c\n%s", optopt, help_text);
      mode = stop;
      code = 1;
      break;
    }
  }
  lyn::compilation_context cc;
  switch (mode) {
  case syntax_only:
    for (int i = optind; i < argc; ++i) {
      input_name = argv[i];
      input = fopen(argv[i], "r");
      if (!input) {
        fprintf(stderr, "error: Could not open input file \"%s\"\n", argv[i]);
        code = 1;
        continue;
      }
      if (!exec_frontend(input, input_name, cc))
        code = 1;
      cc.expr_alloc.release();
      cc.type_alloc.release();
    }
    break;
  case dump_ir:
    if (optind + 1 < argc) {
      fputs(
          "warning: Only the first source file is respected when dumping IR\n",
          stderr);
    }
    if (optind < argc) {
      input = fopen(argv[optind], "r");
      input_name = argv[optind];
      if (!input) {
        fprintf(stderr, "error: Could not open input file \"%s\"\n",
                argv[optind]);
        code = 1;
      } else {
        const auto anf_ctx = exec_frontend(input, input_name, cc);
        if (!anf_ctx) {
          code = 1;
          break;
        }
        lyn::print_anf(*anf_ctx, target);
      }
    }
    break;
  case full_compile:
    if (optind + 1 < argc) {
      fputs(
          "warning: Only the first source file is respected when dumping IR\n",
          stderr);
    }
    if (optind < argc) {
      input = fopen(argv[optind], "r");
      input_name = argv[optind];
      if (!input) {
        fprintf(stderr, "error: Could not open input file \"%s\"\n",
                argv[optind]);
        code = 1;
      } else {
        const auto anf_ctx = exec_frontend(input, input_name, cc);
        if (!anf_ctx) {
          code = 1;
          break;
        }
        lyn::genasm(*anf_ctx, target);
      }
    }
    break;
  }
} catch (const std::exception &e) {
  fprintf(stderr, "%s\n", e.what());
  return -1;
}
