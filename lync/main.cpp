#include <cstdio>
#include <unistd.h>

#include "expr.h"
#include "passes.h"

namespace {

const char help_text[] =
    "Usage: lync [options] <input-file>\n"
    " -o <file>\tSpecifies the output file\n"
    " -d\tDumps the intermediate format instead of generating code\n"
    " -s\tSimply performs a syntax check and exits\n"
    " -h\tPrints this message\n";

std::unique_ptr<lyn::anf_context, lyn::delete_anf> exec_frontend(FILE *input) {
  auto decls = lyn::parse(input);
  auto table = lyn::scopify(decls);
  std::pmr::monotonic_buffer_resource type_pool;
  lyn::typecheck(decls, table, type_pool);
  return lyn::genanf(decls, table);
}

} // namespace

int main(int argc, char **argv) {
  enum mode_t {
    stop,
    syntax_only,
    dump_ir,
    full_compile,
  } mode = full_compile;
  int code = 0;
  FILE *input = nullptr;
  FILE *target = stdout;
  int ret;
  while (ret = getopt(argc, argv, "ho:ds"), ret != -1 && mode != stop) {
    switch (ret) {
    case 'o':
      if (std::string_view("-") == optarg) {
        target = stdout;
      } else {
        target = fopen(optarg, "r");
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
  switch (mode) {
  case syntax_only:
    for (int i = optind; i < argc; ++i) {
      input = fopen(argv[i], "r");
      if (!input) {
        fprintf(stderr, "error: Could not open input file \"%s\"\n", argv[i]);
        code = 1;
        continue;
      }
      exec_frontend(input);
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
      if (!input) {
        fprintf(stderr, "error: Could not open input file \"%s\"\n",
                argv[optind]);
        code = 1;
      } else {
        lyn::print_anf(*exec_frontend(input), target);
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
      if (!input) {
        fprintf(stderr, "error: Could not open input file \"%s\"\n",
                argv[optind]);
        code = 1;
      } else {
        lyn::genasm(*exec_frontend(input), target);
      }
    }
    break;
  }
}
