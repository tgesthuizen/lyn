#include "expr.h"
#include "token.h"

#include <stdexcept>

namespace {

std::unique_ptr<expr> parse_expr(lexer &lex) {}

toplevel_expr parse_define(lexer &lex) {
  if (lex.curtok.kind != token_kind::tok_left_paren) {
    throw std::invalid_argument{"Expected ("};
  }
  lex.lex();
  if (lex.curtok.kind != token_kind::tok_define) {
    throw std::invalid_argument{"Expected \"define\""};
  }
  lex.lex();
  if (lex.curtok.kind != token_kind::tok_identifier) {
    throw std::invalid_argument{"Expected identifier after \"define\""};
  }
  std::string name = std::get<std::string>(std::move(lex.curtok.value));
  lex.lex();
  std::unique_ptr<expr> expr = parse_expr(lex);
  if (lex.curtok.kind != token_kind::tok_right_paren) {
    throw std::invalid_argument{"Expected closing paren"};
  }
  return {std::move(name), std::move(expr)};
}

} // namespace

std::vector<toplevel_expr> parse(FILE *input) {
  lexer lex{input};
  lex.lex();
  std::vector<toplevel_expr> toplevel;
  while (lex.curtok.kind != token_kind::tok_eof) {
    toplevel.push_back(parse_define(lex));
  }
  return toplevel;
}
