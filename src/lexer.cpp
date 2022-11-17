#include "parser.h"
#include "token.h"

#include <algorithm>

namespace lyn {

parser::symbol_type lexer::lex() {
  const auto update_pos = [this](int c) {
    ++col;
    if (c == '\n') {
      ++line;
      col = 1;
    }
  };
  const auto isalpha = [](int c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
  };
  const auto issym = [](int c) {
    const char valid_chars[] = "!$%&*+-./:<=>?@^_~";
    return std::any_of(std::begin(valid_chars), std::end(valid_chars),
                       [c](char c2) { return c == c2; });
  };
  const auto isdigit = [](int c) { return c >= '0' && c <= '9'; };
  const auto isspace = [](int c) { return c == ' ' || c == '\t' || c == '\n'; };
  int c;
  const int tok_line = line;
  const int tok_col = col;
  do {
    c = std::getc(file);
    update_pos(c);
  } while (isspace(c));
  if (isalpha(c) || issym(c)) {
    std::string result;
    result.push_back(c);
    while (c = std::getc(file), isalpha(c) || issym(c) || isdigit(c)) {
      result.push_back(c);
      update_pos(c);
    }
    std::ungetc(c, file);
    if (result == "let") {
      return parser::make_LET({tok_line, tok_col});
    }
    if (result == "lambda") {
      return parser::make_LAMBDA({tok_line, tok_col});
    }
    if (result == "if") {
      return parser::make_IF({tok_line, tok_col});
    }
    if (result == "define") {
      return parser::make_DEFINE({tok_line, tok_col});
    }
    return parser::make_IDENTIFIER(std::move(result), {tok_line, tok_col});
  }
  if (isdigit(c)) {
    int result = c - '0';
    while (c = std::getc(file), isdigit(c)) {
      result = result * 10 + c - '0';
      update_pos(c);
    }
    std::ungetc(c, file);
    return parser::make_NUMBER(result, {tok_line, tok_col});
  }
  if (c == '(') {
    return parser::make_LPAR({tok_line, tok_col});
  }
  if (c == ')') {
    return parser::make_RPAR({tok_line, tok_col});
  }
  if (c == EOF) {
    return parser::make_YYEOF({tok_line, tok_col});
  }
  return parser::make_YYerror({tok_line, tok_col});
}

void parser::error(const location &loc, const std::string &what) {}

std::vector<toplevel_expr> parse(FILE *f) {
  std::vector<toplevel_expr> defines;
  lexer lex{f, 1, 1};
  parser parser{lex, defines};
  // parser.set_debug_level(2);
  if (parser.parse() != 0) {
    return {};
  }
  return defines;
};

} // namespace lyn
