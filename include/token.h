#ifndef COMPILER_TOKEN_H
#define COMPILER_TOKEN_H

#include <cstdio>
#include <string>
#include <variant>

enum class token_kind {
  tok_left_paren,
  tok_right_paren,
  tok_identifier,
  tok_constant,
  tok_let,
  tok_lambda,
  tok_if,
  tok_then,
  tok_else,
  tok_define,
  tok_eof,
};

struct token {
  int line;
  int col;
  token_kind kind;
  std::variant<std::monostate, int, bool, std::string> value;
};

struct lexer {
  FILE *file;
  int line = 1;
  int col = 1;
  token curtok;
  
  void lex();
};

#endif
