#ifndef COMPILER_TOKEN_H
#define COMPILER_TOKEN_H

#include "parser.h"

struct lexer {
  FILE *file;
  int line = 1;
  int col = 1;
  
  yy::parser::symbol_type lex();
};

#endif
