#ifndef LYN_TOKEN_H
#define LYN_TOKEN_H

#include "parser.h"

namespace lyn {

struct lexer {
  FILE *file;
  int line = 1;
  int col = 1;

  parser::symbol_type lex();
};

} // namespace lyn

#endif
