#include "token.h"

#include <stdexcept>

void lexer::lex() {
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
  const auto isdigit = [](int c) { return c >= '0' && c <= '9'; };
  const auto isspace = [](int c) { return c == ' ' || c == '\t' || c == '\n'; };
  int c;
  const int tok_line = line;
  const int tok_col = col;
  using token_value = decltype(std::declval<token>().value);
  do {
    c = std::getc(file);
    update_pos(c);
  } while (isspace(c));
  if (isalpha(c)) {
    std::string result;
    result.push_back(c);
    while (c = std::getc(file), isalpha(c)) {
      result.push_back(c);
      update_pos(c);
    }
    std::ungetc(c, file);
    if (result == "let") {
      curtok = {tok_line, tok_col, token_kind::tok_let, token_value{}};
      return;
    } else if (result == "lambda") {
      curtok = {tok_line, tok_col, token_kind::tok_lambda, token_value{}};
      return;
    } else if (result == "if") {
      curtok = {tok_line, tok_col, token_kind::tok_if, token_value{}};
      return;
    } else if (result == "then") {
      curtok = {tok_line, tok_col, token_kind::tok_then, token_value{}};
      return;
    } else if (result == "else") {
      curtok = {tok_line, tok_col, token_kind::tok_else, token_value{}};
      return;
    } else if (result == "define") {
      curtok = {tok_line, tok_col, token_kind::tok_define, token_value{}};
      return;
    } else {
      curtok = {tok_line, tok_col, token_kind::tok_identifier, result};
      return;
    }
  } else if (isdigit(c)) {
    int result = c - '0';
    while (c = std::getc(file), isdigit(c)) {
      result = result * 10 + c - '0';
      update_pos(c);
    }
    std::ungetc(c, file);
    curtok = {tok_line, tok_col, token_kind::tok_constant, result};
    return;
  } else if (c == '(') {
    curtok = {tok_line, tok_col, token_kind::tok_left_paren, token_value{}};
    return;
  } else if (c == ')') {
    curtok = {tok_line, tok_col, token_kind::tok_right_paren, token_value{}};
    return;
  } else if (c == EOF) {
    curtok = {tok_line, tok_col, token_kind::tok_eof, token_value{}};
    return;
  } else {
    throw std::runtime_error{"Invalid lexer token"};
  }
}
