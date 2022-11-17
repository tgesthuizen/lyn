#include "parser.h"
#include "token.h"

yy::parser::symbol_type lexer::lex() {
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
      return yy::parser::make_LET({tok_line, tok_col});
    } else if (result == "lambda") {
      return yy::parser::make_LAMBDA({tok_line, tok_col});
    } else if (result == "if") {
      return yy::parser::make_IF({tok_line, tok_col});
    } else if (result == "then") {
      return yy::parser::make_THEN({tok_line, tok_col});
    } else if (result == "else") {
      return yy::parser::make_ELSE({tok_line, tok_col});
    } else if (result == "define") {
      return yy::parser::make_DEFINE({tok_line, tok_col});
    } else {
      return yy::parser::make_IDENTIFIER(std::move(result),
                                         {tok_line, tok_col});
    }
  } else if (isdigit(c)) {
    int result = c - '0';
    while (c = std::getc(file), isdigit(c)) {
      result = result * 10 + c - '0';
      update_pos(c);
    }
    std::ungetc(c, file);
    return yy::parser::make_NUMBER(result, {tok_line, tok_col});
  } else if (c == '(') {
    return yy::parser::make_LPAR({tok_line, tok_col});
  } else if (c == ')') {
    return yy::parser::make_RPAR({tok_line, tok_col});
  } else if (c == EOF) {
    return yy::parser::make_EOF({tok_line, tok_col});
  } else {
    return yy::parser::make_YYerror({tok_line, tok_col});
  }
}

void yy::parser::error(const location &loc, const std::string &what) {

}
