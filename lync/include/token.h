#ifndef LYN_TOKEN_H
#define LYN_TOKEN_H

namespace lyn {

struct lexer {
  FILE *file;
  int line = 1;
  int col = 1;

  parser::symbol_type lex();
};

struct driver {
  lexer lex;
  std::vector<toplevel_expr> defines;
};

inline parser::symbol_type yylex(driver &drv) { return drv.lex.lex(); }

} // namespace lyn

#endif
