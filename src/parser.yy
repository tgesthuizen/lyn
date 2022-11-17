%skeleton "lalr1.cc"
%require "3.7.6"

%define api.token.raw
%define api.token.constructor
%define api.value.type variant
%define api.parser.class {parser}
%define api.value.automove true
%define parse.assert
%code requires {
#include "expr.h"
#include "location.h"

#include <memory>

class lexer;
}
%code {
#include "token.h"
yy::parser::symbol_type yylex(lexer &lex) {
  return lex.lex();
}
}
%locations
%define api.location.type {location}
%param { lexer &lex }

%define api.token.prefix {TOK_}
%token
  LPAR
  RPAR
  LET
  BEGIN
  LAMBDA
  IF
  THEN
  ELSE
  DEFINE
  EOF
;
%token <std::string> IDENTIFIER "identifier"
%token <int> NUMBER "number"

%nterm <std::vector<toplevel_expr>> defines
%nterm <toplevel_expr> define
%nterm <std::unique_ptr<expr>> expr
%nterm <std::vector<let_binding>> bindings
%nterm <let_binding> binding
%nterm <std::vector<std::unique_ptr<expr>>> expr-list
%nterm <std::vector<variable_expr>> ident-list

%%
%start document;

document: defines

defines: %empty                                  {}
       | defines define                          { $$ = std::move($1); $$.push_back(std::move($2)); }
define: LPAR DEFINE IDENTIFIER expr RPAR         { $$ = {std::move($3), std::move($4)}; }

expr: IDENTIFIER                                 { $$ = std::make_unique<expr>(variable_expr{std::move($1)}, @$.line, @$.col); }
    | NUMBER                                     { $$ = std::make_unique<expr>(constant_expr{constant_type::Int, $1}, @$.line, @$.col); }
    | LPAR expr expr-list RPAR                   { $$ = std::make_unique<expr>(apply_expr{std::move($2), std::move($3)}, @$.line, @$.col); }
    | LPAR LET LPAR bindings RPAR expr RPAR      { $$ = std::make_unique<expr>(let_expr{std::move($4), std::move($6)}, @$.line, @$.col); }
    | LPAR IF expr THEN expr ELSE expr RPAR      { $$ = std::make_unique<expr>(if_expr{std::move($3), std::move($5), std::move($7)}, @$.line, @$.col); }
    | LPAR LAMBDA LPAR ident-list RPAR expr RPAR { $$ = std::make_unique<expr>(lambda_expr{std::move($4), std::move($6)}, @$.line, @$.col); }
    | LPAR BEGIN expr-list RPAR                  { $$ = std::make_unique<expr>(begin_expr{std::move($3)}, @$.line, @$.col); }

bindings: %empty                                 {}
        | bindings binding                       { $$ = std::move($1); $$.push_back(std::move($2)); }

binding: LPAR IDENTIFIER expr RPAR               { $$ = {std::move($2), 0, std::move($3)}; }

expr-list: %empty                                {}
	 | expr-list expr                        { $$ = std::move($1); $$.push_back(std::move($2)); }

ident-list: %empty                               {}
          | ident-list IDENTIFIER                { $$ = std::move($1); $$.push_back(variable_expr{std::move($2), 0}); }
