#include "expr.h"
#include "passes.h"
#include <algorithm>
#include <optional>
#include <string_view>

namespace lyn {

namespace {

struct token {
  enum class type {
    error,
    eof,
    lpar,
    rpar,
    arrow,
    let,
    rec,
    lambda,
    if_,
    define,
    declare,
    include,
    identifier,
    number,
  };

  type t = type::error;
  union {
    int i;
    std::string_view s;
  } value;
  source_location sloc = {};
};

void print_token(const token &tok) {
  switch (tok.t) {
  case token::type::error:
    fputs("<error>", stderr);
    break;
  case token::type::eof:
    fputs("<eof>", stderr);
    break;
  case token::type::lpar:
    fputc('(', stderr);
    break;
  case token::type::rpar:
    fputc(')', stderr);
    break;
  case token::type::arrow:
    fputs("->", stderr);
    break;
  case token::type::let:
    fputs("let", stderr);
    break;
  case token::type::rec:
    fputs("rec", stderr);
    break;
  case token::type::lambda:
    fputs("lambda", stderr);
    break;
  case token::type::if_:
    fputs("if", stderr);
    break;
  case token::type::define:
    fputs("define", stderr);
    break;
  case token::type::declare:
    fputs("declare", stderr);
    break;
  case token::type::include:
    fputs("include", stderr);
    break;
  case token::type::identifier:
    fprintf(stderr, "\"%.*s\"", static_cast<int>(std::size(tok.value.s)),
            std::data(tok.value.s));
    break;
  case token::type::number:
    fprintf(stderr, "%d", tok.value.i);
    break;
  }
}

struct include_return {
  FILE *file;
  source_location sloc;
};

struct parse_context {
  FILE *file;
  source_location sloc;
  compilation_context &cc;
  std::vector<include_return> returns = {};
  token cur_tok = {};
  std::vector<toplevel_expr> defines = {};
};

void lex(parse_context &ctx) {
  const auto update_pos = [&](int c) {
    ++ctx.sloc.col;
    if (c == '\n') {
      ++ctx.sloc.line;
      ctx.sloc.col = 1;
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
  ctx.cur_tok.sloc = ctx.sloc;
  do {
    c = std::getc(ctx.file);
    update_pos(c);
  } while (isspace(c));
  if (isalpha(c) || issym(c)) {
    std::string result;
    result.push_back(c);
    while (c = std::getc(ctx.file), isalpha(c) || issym(c) || isdigit(c)) {
      result.push_back(c);
      update_pos(c);
    }
    std::ungetc(c, ctx.file);
    if (result == "->") {
      ctx.cur_tok.t = token::type::arrow;
      return;
    }
    if (result == "let") {
      ctx.cur_tok.t = token::type::let;
      return;
    }
    if(result == "rec") {
      ctx.cur_tok.t = token::type::rec;
      return;
    }
    if (result == "lambda") {
      ctx.cur_tok.t = token::type::lambda;
      return;
    }
    if (result == "if") {
      ctx.cur_tok.t = token::type::if_;
      return;
    }
    if (result == "define") {
      ctx.cur_tok.t = token::type::define;
      return;
    }
    if (result == "declare") {
      ctx.cur_tok.t = token::type::declare;
      return;
    }
    if (result == "include") {
      ctx.cur_tok.t = token::type::include;
      return;
    }
    ctx.cur_tok.t = token::type::identifier;
    ctx.cur_tok.value.s = ctx.cc.stbl.store(result);
    return;
  }
  if (isdigit(c)) {
    int result = c - '0';
    while (c = std::getc(ctx.file), isdigit(c)) {
      result = result * 10 + c - '0';
      update_pos(c);
    }
    std::ungetc(c, ctx.file);
    ctx.cur_tok.t = token::type::number;
    ctx.cur_tok.value.i = result;
    return;
  }
  if (c == '(') {
    ctx.cur_tok.t = token::type::lpar;
    return;
  }
  if (c == ')') {
    ctx.cur_tok.t = token::type::rpar;
    return;
  }
  if (c == EOF) {
    if (std::empty(ctx.returns)) {
      ctx.cur_tok.t = token::type::eof;
      return;
    } else {
      fclose(ctx.file);
      ctx.file = ctx.returns.back().file;
      ctx.sloc = ctx.returns.back().sloc;
      lex(ctx);
      return;
    }
  }
  ctx.cur_tok.t = token::type::error;
}

template <class... Args>
expr *make_expr(compilation_context &cc, Args &&...args) {
  return new (cc.expr_alloc.allocate(sizeof(expr), alignof(expr)))
      expr{std::forward<Args>(args)...};
}

expr *parse_expr(parse_context &ctx);

expr *parse_lambda(parse_context &ctx, const source_location &sloc) {
  lambda_expr res;
  lex(ctx);
  if (ctx.cur_tok.t != token::type::lpar) {
    fprintf(stderr, "%.*s:%d:%d: error: Expected parameter list\n",
            static_cast<int>(std::size(ctx.sloc.file_name)),
            std::data(ctx.sloc.file_name), ctx.sloc.line, ctx.sloc.col);
    return nullptr;
  }
  lex(ctx);
  std::vector<variable_expr> args;
  while (ctx.cur_tok.t != token::type::rpar) {
    if (ctx.cur_tok.t != token::type::identifier) {
      fprintf(stderr, "%.*s:%d:%d: error: Expected parameter name\n",
              static_cast<int>(std::size(ctx.sloc.file_name)),
              std::data(ctx.sloc.file_name), ctx.sloc.line, ctx.sloc.col);
      return nullptr;
    }
    args.push_back(variable_expr{ctx.cur_tok.value.s});
    lex(ctx);
  }
  res.params = spanify(ctx.cc.expr_alloc, args);
  lex(ctx);
  res.body = parse_expr(ctx);
  if (!res.body)
    return nullptr;
  if (ctx.cur_tok.t != token::type::rpar) {
    fprintf(stderr, "%.*s:%d:%d: error: Expected closing paren after lambda\n",
            static_cast<int>(std::size(ctx.sloc.file_name)),
            std::data(ctx.sloc.file_name), ctx.sloc.line, ctx.sloc.col);
    return nullptr;
  }
  lex(ctx);
  return make_expr(ctx.cc, std::move(res), sloc);
}

expr *parse_let(parse_context &ctx, const source_location &sloc) {
  let_expr res;
  lex(ctx);
  if(ctx.cur_tok.t == token::type::rec) {
    res.recursive = true;
    lex(ctx);
  }
  if (ctx.cur_tok.t != token::type::lpar) {
    fprintf(stderr, "%.*s:%d:%d: error: Expected let binding list\n",
            static_cast<int>(std::size(ctx.sloc.file_name)),
            std::data(ctx.sloc.file_name), ctx.sloc.line, ctx.sloc.col);
    return nullptr;
  }
  lex(ctx);
  std::vector<let_binding> bindings;
  while (ctx.cur_tok.t != token::type::rpar) {
    let_binding b;
    if (ctx.cur_tok.t != token::type::lpar) {
      fprintf(stderr, "%.*s:%d:%d: error: Expected let binding\n",
              static_cast<int>(std::size(ctx.sloc.file_name)),
              std::data(ctx.sloc.file_name), ctx.sloc.line, ctx.sloc.col);
      return nullptr;
    }
    lex(ctx);
    if (ctx.cur_tok.t != token::type::identifier) {
      fprintf(stderr, "%.*s:%d:%d: error: Expected let binding name\n",
              static_cast<int>(std::size(ctx.sloc.file_name)),
              std::data(ctx.sloc.file_name), ctx.sloc.line, ctx.sloc.col);
      return nullptr;
    }
    b.name = ctx.cur_tok.value.s;
    lex(ctx);
    b.body = parse_expr(ctx);
    if (!b.body)
      return nullptr;
    if (ctx.cur_tok.t != token::type::rpar) {
      fprintf(stderr, "%.*s:%d:%d: error: Expected closing paren after let\n",
              static_cast<int>(std::size(ctx.sloc.file_name)),
              std::data(ctx.sloc.file_name), ctx.sloc.line, ctx.sloc.col);
      return nullptr;
    }
    lex(ctx);
    bindings.push_back(std::move(b));
  }
  lex(ctx);
  res.bindings = spanify(ctx.cc.expr_alloc, bindings);
  std::vector<expr *> exprs;
  while (ctx.cur_tok.t != token::type::rpar) {
    exprs.push_back(parse_expr(ctx));
    if (!exprs.back())
      return nullptr;
  }
  res.body = spanify(ctx.cc.expr_alloc, exprs);
  lex(ctx);
  return make_expr(ctx.cc, std::move(res), sloc);
}

expr *parse_if(parse_context &ctx, const source_location &sloc) {
  if_expr res;
  lex(ctx);
  res.cond = parse_expr(ctx);
  if (!res.cond)
    return nullptr;
  res.then = parse_expr(ctx);
  if (!res.then)
    return nullptr;
  res.els = parse_expr(ctx);
  if (!res.els)
    return nullptr;
  if (ctx.cur_tok.t != token::type::rpar) {
    fprintf(stderr,
            "%.*s:%d:%d: error: Expected closing paren after conditional\n",
            static_cast<int>(std::size(ctx.sloc.file_name)),
            std::data(ctx.sloc.file_name), ctx.sloc.line, ctx.sloc.col);
    return nullptr;
  }
  lex(ctx);
  return make_expr(ctx.cc, std::move(res), sloc);
}

expr *parse_expr(parse_context &ctx) {
  switch (ctx.cur_tok.t) {
  case token::type::number: {
    auto res =
        make_expr(ctx.cc, constant_expr{ctx.cur_tok.value.i}, ctx.cur_tok.sloc);
    lex(ctx);
    return std::move(res);
  }
  case token::type::identifier: {
    auto res =
        make_expr(ctx.cc, variable_expr{ctx.cur_tok.value.s}, ctx.cur_tok.sloc);
    lex(ctx);
    return std::move(res);
  }
  case token::type::lpar: {
    const auto sloc = ctx.cur_tok.sloc;
    lex(ctx);
    switch (ctx.cur_tok.t) {
    case token::type::lambda:
      return parse_lambda(ctx, sloc);
    case token::type::let:
      return parse_let(ctx, sloc);
    case token::type::if_:
      return parse_if(ctx, sloc);
    default:
      apply_expr res;
      res.func = parse_expr(ctx);
      if (!res.func)
        return nullptr;
      std::vector<expr *> args;
      while (ctx.cur_tok.t != token::type::rpar) {
        expr *arg_expr = parse_expr(ctx);
        if (!arg_expr)
          return nullptr;
        args.push_back(arg_expr);
      }
      res.args = spanify(ctx.cc.expr_alloc, args);
      lex(ctx);
      return make_expr(ctx.cc, std::move(res), sloc);
    }
  }
  case token::type::error:
  case token::type::eof:
  case token::type::rpar:
  case token::type::arrow:
  case token::type::let:
  case token::type::lambda:
  case token::type::if_:
  case token::type::define:
  case token::type::declare:
  case token::type::include:
    fprintf(stderr, "%.*s:%d:%d: error: Unexpected token ",
            static_cast<int>(std::size(ctx.sloc.file_name)),
            std::data(ctx.sloc.file_name), ctx.sloc.line, ctx.sloc.col);
    print_token(ctx.cur_tok);
    fputc('\n', stderr);
    return nullptr;
  }
  unreachable();
}

bool parse_def(parse_context &ctx) {
  lex(ctx);
  if (ctx.cur_tok.t != token::type::identifier) {
    fprintf(stderr, "%.*s:%d:%d: error: Expected definition name\n",
            static_cast<int>(std::size(ctx.sloc.file_name)),
            std::data(ctx.sloc.file_name), ctx.sloc.line, ctx.sloc.col);
    return false;
  }
  const std::string_view name = ctx.cur_tok.value.s;
  lex(ctx);
  expr *const ptr = parse_expr(ctx);
  if (!ptr) {
    return false;
  }
  const auto end = std::end(ctx.defines);
  const auto iter = std::find_if(
      std::begin(ctx.defines), end,
      [name](const toplevel_expr &expr) { return expr.name == name; });
  if (iter == end)
    ctx.defines.push_back(toplevel_expr{name, 0, nullptr, ptr});
  else if (iter->value) {
    fprintf(stderr, "%.*s:%d:%d: error: Duplicate definition of \"%.*s\"\n",
            static_cast<int>(std::size(ctx.sloc.file_name)),
            std::data(ctx.sloc.file_name), ctx.sloc.line, ctx.sloc.col,
            static_cast<int>(std::size(name)), std::data(name));
    return false;
  } else
    iter->value = ptr;
  if (ctx.cur_tok.t != token::type::rpar) {
    fprintf(
        stderr,
        "%.*s:%d:%d: error: Expected closing paren after closing definition\n",
        static_cast<int>(std::size(ctx.sloc.file_name)),
        std::data(ctx.sloc.file_name), ctx.sloc.line, ctx.sloc.col);
    return false;
  }
  lex(ctx);
  return true;
}

type_expr *make_type_expr(parse_context &ctx, const type_expr &expr) {
  return new (ctx.cc.expr_alloc.allocate(sizeof(type_expr), alignof(type_expr)))
      type_expr{expr};
}

type_expr *parse_type_expr(parse_context &ctx) {
  if (ctx.cur_tok.t == token::type::identifier) {
    const auto ident = ctx.cur_tok.value.s;
    if (ident == "int") {
      return make_type_expr(ctx, type_expr{int_type_expr{}});
    }
    if (ident == "bool")
      return make_type_expr(ctx, type_expr{bool_type_expr{}});
    if (ident == "unit")
      return make_type_expr(ctx, type_expr{unit_type_expr{}});
  }
  if (ctx.cur_tok.t == token::type::lpar) {
    lex(ctx);
    if (ctx.cur_tok.t != token::type::arrow) {
      return nullptr;
    }
    lex(ctx);
    std::vector<type_expr *> types;
    while (ctx.cur_tok.t != token::type::rpar) {
      types.push_back(parse_type_expr(ctx));
      lex(ctx);
    }
    lex(ctx);
    return make_type_expr(
        ctx, type_expr{func_type_expr{spanify(ctx.cc.expr_alloc, types)}});
  }
  return nullptr;
}

bool parse_decl(parse_context &ctx) {
  lex(ctx);
  if (ctx.cur_tok.t != token::type::identifier) {
    return false;
  }
  const std::string_view name = ctx.cur_tok.value.s;
  lex(ctx);
  type_expr *const ptr = parse_type_expr(ctx);
  if (!ptr) {
    return false;
  }
  if (ctx.cur_tok.t != token::type::rpar) {
    return false;
  }
  lex(ctx);
  const auto end = std::end(ctx.defines);
  const auto iter = std::find_if(
      std::begin(ctx.defines), end,
      [name](const toplevel_expr &expr) { return expr.name == name; });
  if (iter == end)
    ctx.defines.push_back(toplevel_expr{name, 0, ptr, nullptr});
  else if (iter->type_value)
    return false;
  else
    iter->type_value = ptr;
  return true;
}

bool parse_include(parse_context &ctx) {
  lex(ctx);
  if (ctx.cur_tok.t != token::type::identifier)
    return false;
  const std::string_view include_file = ctx.cur_tok.value.s;
  lex(ctx);
  if (ctx.cur_tok.t != token::type::rpar)
    return false;
  ctx.returns.push_back({ctx.file, ctx.sloc});
  ctx.file = std::fopen(std::string{include_file}.c_str(), "r");
  if (!ctx.file)
    return false;
  ctx.sloc = {include_file};
  lex(ctx);
  return true;
}

bool parse_toplevel(parse_context &ctx) {
  while (ctx.cur_tok.t == token::type::lpar) {
    lex(ctx);
    if (ctx.cur_tok.t == token::type::define && !parse_def(ctx))
      return false;
    if (ctx.cur_tok.t == token::type::declare && !parse_decl(ctx))
      return false;
    if (ctx.cur_tok.t == token::type::include && !parse_include(ctx))
      return false;
  }
  return ctx.cur_tok.t == token::type::eof;
}

} // namespace

std::optional<std::vector<toplevel_expr>>
parse(FILE *f, std::string_view file_name, compilation_context &cc) {
  parse_context ctx{f, {file_name, 1, 1}, cc};
  lex(ctx);
  if (!parse_toplevel(ctx))
    return std::nullopt;
  return std::move(ctx.defines);
}

} // namespace lyn
