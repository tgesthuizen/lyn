// A Bison parser, made by GNU Bison 3.7.6.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2021 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.





#include "parser.hpp"




#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif


// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (false)
# endif


// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yy_stack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YY_USE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

namespace yy {
#line 136 "src/parser.cpp"

  /// Build a parser object.
  parser::parser ()
#if YYDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
#else
    :
#endif
      yy_lac_established_ (false)
  {}

  parser::~parser ()
  {}

  parser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------------.
  | symbol kinds.  |
  `---------------*/



  // by_state.
  parser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  parser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  parser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  parser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  parser::symbol_kind_type
  parser::by_state::kind () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return symbol_kind::S_YYEMPTY;
    else
      return YY_CAST (symbol_kind_type, yystos_[+state]);
  }

  parser::stack_symbol_type::stack_symbol_type ()
  {}

  parser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_NUMBER: // "number"
        value.YY_MOVE_OR_COPY< int > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_binding: // binding
        value.YY_MOVE_OR_COPY< let_binding > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_IDENTIFIER: // "identifier"
        value.YY_MOVE_OR_COPY< std::string > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_expr: // expr
        value.YY_MOVE_OR_COPY< std::unique_ptr<expr> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_bindings: // bindings
        value.YY_MOVE_OR_COPY< std::vector<let_binding> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_22_expr_list: // expr-list
        value.YY_MOVE_OR_COPY< std::vector<std::unique_ptr<expr>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_defines: // defines
        value.YY_MOVE_OR_COPY< std::vector<toplevel_expr> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_23_ident_list: // ident-list
        value.YY_MOVE_OR_COPY< std::vector<variable_expr> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_define: // define
        value.YY_MOVE_OR_COPY< toplevel_expr > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  parser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_NUMBER: // "number"
        value.move< int > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_binding: // binding
        value.move< let_binding > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_IDENTIFIER: // "identifier"
        value.move< std::string > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_expr: // expr
        value.move< std::unique_ptr<expr> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_bindings: // bindings
        value.move< std::vector<let_binding> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_22_expr_list: // expr-list
        value.move< std::vector<std::unique_ptr<expr>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_defines: // defines
        value.move< std::vector<toplevel_expr> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_23_ident_list: // ident-list
        value.move< std::vector<variable_expr> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_define: // define
        value.move< toplevel_expr > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_NUMBER: // "number"
        value.copy< int > (that.value);
        break;

      case symbol_kind::S_binding: // binding
        value.copy< let_binding > (that.value);
        break;

      case symbol_kind::S_IDENTIFIER: // "identifier"
        value.copy< std::string > (that.value);
        break;

      case symbol_kind::S_expr: // expr
        value.copy< std::unique_ptr<expr> > (that.value);
        break;

      case symbol_kind::S_bindings: // bindings
        value.copy< std::vector<let_binding> > (that.value);
        break;

      case symbol_kind::S_22_expr_list: // expr-list
        value.copy< std::vector<std::unique_ptr<expr>> > (that.value);
        break;

      case symbol_kind::S_defines: // defines
        value.copy< std::vector<toplevel_expr> > (that.value);
        break;

      case symbol_kind::S_23_ident_list: // ident-list
        value.copy< std::vector<variable_expr> > (that.value);
        break;

      case symbol_kind::S_define: // define
        value.copy< toplevel_expr > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    return *this;
  }

  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_NUMBER: // "number"
        value.move< int > (that.value);
        break;

      case symbol_kind::S_binding: // binding
        value.move< let_binding > (that.value);
        break;

      case symbol_kind::S_IDENTIFIER: // "identifier"
        value.move< std::string > (that.value);
        break;

      case symbol_kind::S_expr: // expr
        value.move< std::unique_ptr<expr> > (that.value);
        break;

      case symbol_kind::S_bindings: // bindings
        value.move< std::vector<let_binding> > (that.value);
        break;

      case symbol_kind::S_22_expr_list: // expr-list
        value.move< std::vector<std::unique_ptr<expr>> > (that.value);
        break;

      case symbol_kind::S_defines: // defines
        value.move< std::vector<toplevel_expr> > (that.value);
        break;

      case symbol_kind::S_23_ident_list: // ident-list
        value.move< std::vector<variable_expr> > (that.value);
        break;

      case symbol_kind::S_define: // define
        value.move< toplevel_expr > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
  parser::yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YY_USE (yyoutput);
    if (yysym.empty ())
      yyo << "empty symbol";
    else
      {
        symbol_kind_type yykind = yysym.kind ();
        yyo << (yykind < YYNTOKENS ? "token" : "nterm")
            << ' ' << yysym.name () << " ("
            << yysym.location << ": ";
        YY_USE (yykind);
        yyo << ')';
      }
  }
#endif

  void
  parser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  parser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  parser::yypop_ (int n)
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  parser::debug_level_type
  parser::debug_level () const
  {
    return yydebug_;
  }

  void
  parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  parser::state_type
  parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - YYNTOKENS];
  }

  bool
  parser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  parser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  parser::operator() ()
  {
    return parse ();
  }

  int
  parser::parse ()
  {
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

    /// Discard the LAC context in case there still is one left from a
    /// previous invocation.
    yy_lac_discard_ ("init");

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << int (yystack_[0].state) << '\n';
    YY_STACK_PRINT ();

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[+yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token\n";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            symbol_type yylookahead (yylex ());
            yyla.move (yylookahead);
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    if (yyla.kind () == symbol_kind::S_YYerror)
    {
      // The scanner already issued an error message, process directly
      // to error recovery.  But do not keep the error token as
      // lookahead, it is too special and may lead us to an endless
      // loop in error recovery. */
      yyla.kind_ = symbol_kind::S_YYUNDEF;
      goto yyerrlab1;
    }

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.kind ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.kind ())
      {
        if (!yy_lac_establish_ (yyla.kind ()))
           goto yyerrlab;
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        if (!yy_lac_establish_ (yyla.kind ()))
           goto yyerrlab;

        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    yy_lac_discard_ ("shift");
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[+yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
      switch (yyr1_[yyn])
    {
      case symbol_kind::S_NUMBER: // "number"
        yylhs.value.emplace< int > ();
        break;

      case symbol_kind::S_binding: // binding
        yylhs.value.emplace< let_binding > ();
        break;

      case symbol_kind::S_IDENTIFIER: // "identifier"
        yylhs.value.emplace< std::string > ();
        break;

      case symbol_kind::S_expr: // expr
        yylhs.value.emplace< std::unique_ptr<expr> > ();
        break;

      case symbol_kind::S_bindings: // bindings
        yylhs.value.emplace< std::vector<let_binding> > ();
        break;

      case symbol_kind::S_22_expr_list: // expr-list
        yylhs.value.emplace< std::vector<std::unique_ptr<expr>> > ();
        break;

      case symbol_kind::S_defines: // defines
        yylhs.value.emplace< std::vector<toplevel_expr> > ();
        break;

      case symbol_kind::S_23_ident_list: // ident-list
        yylhs.value.emplace< std::vector<variable_expr> > ();
        break;

      case symbol_kind::S_define: // define
        yylhs.value.emplace< toplevel_expr > ();
        break;

      default:
        break;
    }


      // Default location.
      {
        stack_type::slice range (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, range, yylen);
        yyerror_range[1].location = yylhs.location;
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 3: // defines: defines define
#line 46 "src/parser.yy"
                                                 { yylhs.value.as < std::vector<toplevel_expr> > () = std::move(yystack_[1].value.as < std::vector<toplevel_expr> > ()); yylhs.value.as < std::vector<toplevel_expr> > ().push_back(std::move(yystack_[0].value.as < toplevel_expr > ())); }
#line 720 "src/parser.cpp"
    break;

  case 4: // defines: %empty
#line 47 "src/parser.yy"
                                                 { yylhs.value.as < std::vector<toplevel_expr> > () = {}; }
#line 726 "src/parser.cpp"
    break;

  case 5: // define: LPAR DEFINE "identifier" expr RPAR
#line 48 "src/parser.yy"
                                                 { yylhs.value.as < toplevel_expr > () = {std::move(yystack_[2].value.as < std::string > ()), std::move(yystack_[1].value.as < std::unique_ptr<expr> > ())}; }
#line 732 "src/parser.cpp"
    break;

  case 6: // expr: "identifier"
#line 50 "src/parser.yy"
                                                 { yylhs.value.as < std::unique_ptr<expr> > () = std::make_unique<expr>(variable_expr{std::move(yystack_[0].value.as < std::string > ())}); }
#line 738 "src/parser.cpp"
    break;

  case 7: // expr: "number"
#line 51 "src/parser.yy"
                                                 { yylhs.value.as < std::unique_ptr<expr> > () = std::make_unique<expr>(constant_expr{constant_type::Int, yystack_[0].value.as < int > ()}); }
#line 744 "src/parser.cpp"
    break;

  case 8: // expr: LPAR expr expr-list RPAR
#line 52 "src/parser.yy"
                                                 { yylhs.value.as < std::unique_ptr<expr> > () = std::make_unique<expr>(apply_expr{std::move(yystack_[2].value.as < std::unique_ptr<expr> > ()), std::move(yystack_[1].value.as < std::vector<std::unique_ptr<expr>> > ())}); }
#line 750 "src/parser.cpp"
    break;

  case 9: // expr: LPAR LET LPAR bindings RPAR expr RPAR
#line 53 "src/parser.yy"
                                                 { yylhs.value.as < std::unique_ptr<expr> > () = std::make_unique<expr>(let_expr{std:::move(yystack_[3].value.as < std::vector<let_binding> > ()), std::move(yystack_[1].value.as < std::unique_ptr<expr> > ())}); }
#line 756 "src/parser.cpp"
    break;

  case 10: // expr: LPAR IF expr THEN expr ELSE expr RPAR
#line 54 "src/parser.yy"
                                                 { yylhs.value.as < std::unique_ptr<expr> > () = std::make_unique<expr>(if_expr{std::move(yystack_[5].value.as < std::unique_ptr<expr> > ()), std::move(yystack_[3].value.as < std::unique_ptr<expr> > ()), std::move(yystack_[1].value.as < std::unique_ptr<expr> > ())}); }
#line 762 "src/parser.cpp"
    break;

  case 11: // expr: LPAR LAMBDA LPAR ident-list RPAR expr RPAR
#line 55 "src/parser.yy"
                                                 { yylhs.value.as < std::unique_ptr<expr> > () = std::make_unique<expr>(lambda_expr{std::move(yystack_[3].value.as < std::vector<variable_expr> > ()), std::move(yystack_[1].value.as < std::unique_ptr<expr> > ())}); }
#line 768 "src/parser.cpp"
    break;

  case 12: // expr: LPAR BEGIN expr-list RPAR
#line 56 "src/parser.yy"
                                                 { yylhs.value.as < std::unique_ptr<expr> > () = std::make_unique<expr>(begin_expr{std::move(yystack_[1].value.as < std::vector<std::unique_ptr<expr>> > ())}); }
#line 774 "src/parser.cpp"
    break;

  case 13: // bindings: bindings binding
#line 58 "src/parser.yy"
                                                 { yylhs.value.as < std::vector<let_binding> > () = std::move(yystack_[1].value.as < std::vector<let_binding> > ()); yylhs.value.as < std::vector<let_binding> > ().push_back(std::move(yystack_[0].value.as < let_binding > ())); }
#line 780 "src/parser.cpp"
    break;

  case 14: // bindings: %empty
#line 59 "src/parser.yy"
                                                 { yylhs.value.as < std::vector<let_binding> > () = {}; }
#line 786 "src/parser.cpp"
    break;

  case 15: // binding: LPAR "identifier" expr RPAR
#line 61 "src/parser.yy"
                                                 { yylhs.value.as < let_binding > () = {std::move(yystack_[2].value.as < std::string > ()), 0, std::move(yystack_[1].value.as < std::unique_ptr<expr> > ())}; }
#line 792 "src/parser.cpp"
    break;

  case 16: // expr-list: expr-list expr
#line 63 "src/parser.yy"
                                                 { yylhs.value.as < std::vector<std::unique_ptr<expr>> > () = std::move(yystack_[1].value.as < std::vector<std::unique_ptr<expr>> > ()); yylhs.value.as < std::vector<std::unique_ptr<expr>> > ().push_back(std::move(yystack_[0].value.as < std::unique_ptr<expr> > ())); }
#line 798 "src/parser.cpp"
    break;

  case 17: // expr-list: %empty
#line 64 "src/parser.yy"
                                                 { yylhs.value.as < std::vector<std::unique_ptr<expr>> > () = {}; }
#line 804 "src/parser.cpp"
    break;

  case 18: // ident-list: ident-list "identifier"
#line 66 "src/parser.yy"
                                                 { yylhs.value.as < std::vector<variable_expr> > () = std::move(yystack_[1].value.as < std::vector<variable_expr> > ()); yylhs.value.as < std::vector<variable_expr> > ().push_back(std::move(yystack_[0].value.as < std::string > ())); }
#line 810 "src/parser.cpp"
    break;

  case 19: // ident-list: %empty
#line 67 "src/parser.yy"
                                                 { yylhs.value.as < std::vector<variable_expr> > () = {}; }
#line 816 "src/parser.cpp"
    break;


#line 820 "src/parser.cpp"

            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        context yyctx (*this, yyla);
        std::string msg = yysyntax_error_ (yyctx);
        error (yyla.location, YY_MOVE (msg));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.kind () == symbol_kind::S_YYEOF)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    // Pop stack until we find a state that shifts the error token.
    for (;;)
      {
        yyn = yypact_[+yystack_[0].state];
        if (!yy_pact_value_is_default_ (yyn))
          {
            yyn += symbol_kind::S_YYerror;
            if (0 <= yyn && yyn <= yylast_
                && yycheck_[yyn] == symbol_kind::S_YYerror)
              {
                yyn = yytable_[yyn];
                if (0 < yyn)
                  break;
              }
          }

        // Pop the current state because it cannot handle the error token.
        if (yystack_.size () == 1)
          YYABORT;

        yyerror_range[1].location = yystack_[0].location;
        yy_destroy_ ("Error: popping", yystack_[0]);
        yypop_ ();
        YY_STACK_PRINT ();
      }
    {
      stack_symbol_type error_token;

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      yy_lac_discard_ ("error recovery");
      error_token.state = state_type (yyn);
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;


  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
  yyabortlab:
    yyresult = 1;
    goto yyreturn;


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    YY_STACK_PRINT ();
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  parser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what ());
  }

  const char *
  parser::symbol_name (symbol_kind_type yysymbol)
  {
    static const char *const yy_sname[] =
    {
    "end of file", "error", "invalid token", "LPAR", "RPAR", "LET", "BEGIN",
  "LAMBDA", "IF", "THEN", "ELSE", "DEFINE", "EOF", "identifier", "number",
  "$accept", "document", "defines", "define", "expr", "bindings",
  "binding", "expr-list", "ident-list", YY_NULLPTR
    };
    return yy_sname[yysymbol];
  }



  // parser::context.
  parser::context::context (const parser& yyparser, const symbol_type& yyla)
    : yyparser_ (yyparser)
    , yyla_ (yyla)
  {}

  int
  parser::context::expected_tokens (symbol_kind_type yyarg[], int yyargn) const
  {
    // Actual number of expected tokens
    int yycount = 0;

#if YYDEBUG
    // Execute LAC once. We don't care if it is successful, we
    // only do it for the sake of debugging output.
    if (!yyparser_.yy_lac_established_)
      yyparser_.yy_lac_check_ (yyla_.kind ());
#endif

    for (int yyx = 0; yyx < YYNTOKENS; ++yyx)
      {
        symbol_kind_type yysym = YY_CAST (symbol_kind_type, yyx);
        if (yysym != symbol_kind::S_YYerror
            && yysym != symbol_kind::S_YYUNDEF
            && yyparser_.yy_lac_check_ (yysym))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = yysym;
          }
      }
    if (yyarg && yycount == 0 && 0 < yyargn)
      yyarg[0] = symbol_kind::S_YYEMPTY;
    return yycount;
  }


  bool
  parser::yy_lac_check_ (symbol_kind_type yytoken) const
  {
    // Logically, the yylac_stack's lifetime is confined to this function.
    // Clear it, to get rid of potential left-overs from previous call.
    yylac_stack_.clear ();
    // Reduce until we encounter a shift and thereby accept the token.
#if YYDEBUG
    YYCDEBUG << "LAC: checking lookahead " << symbol_name (yytoken) << ':';
#endif
    std::ptrdiff_t lac_top = 0;
    while (true)
      {
        state_type top_state = (yylac_stack_.empty ()
                                ? yystack_[lac_top].state
                                : yylac_stack_.back ());
        int yyrule = yypact_[+top_state];
        if (yy_pact_value_is_default_ (yyrule)
            || (yyrule += yytoken) < 0 || yylast_ < yyrule
            || yycheck_[yyrule] != yytoken)
          {
            // Use the default action.
            yyrule = yydefact_[+top_state];
            if (yyrule == 0)
              {
                YYCDEBUG << " Err\n";
                return false;
              }
          }
        else
          {
            // Use the action from yytable.
            yyrule = yytable_[yyrule];
            if (yy_table_value_is_error_ (yyrule))
              {
                YYCDEBUG << " Err\n";
                return false;
              }
            if (0 < yyrule)
              {
                YYCDEBUG << " S" << yyrule << '\n';
                return true;
              }
            yyrule = -yyrule;
          }
        // By now we know we have to simulate a reduce.
        YYCDEBUG << " R" << yyrule - 1;
        // Pop the corresponding number of values from the stack.
        {
          std::ptrdiff_t yylen = yyr2_[yyrule];
          // First pop from the LAC stack as many tokens as possible.
          std::ptrdiff_t lac_size = std::ptrdiff_t (yylac_stack_.size ());
          if (yylen < lac_size)
            {
              yylac_stack_.resize (std::size_t (lac_size - yylen));
              yylen = 0;
            }
          else if (lac_size)
            {
              yylac_stack_.clear ();
              yylen -= lac_size;
            }
          // Only afterwards look at the main stack.
          // We simulate popping elements by incrementing lac_top.
          lac_top += yylen;
        }
        // Keep top_state in sync with the updated stack.
        top_state = (yylac_stack_.empty ()
                     ? yystack_[lac_top].state
                     : yylac_stack_.back ());
        // Push the resulting state of the reduction.
        state_type state = yy_lr_goto_state_ (top_state, yyr1_[yyrule]);
        YYCDEBUG << " G" << int (state);
        yylac_stack_.push_back (state);
      }
  }

  // Establish the initial context if no initial context currently exists.
  bool
  parser::yy_lac_establish_ (symbol_kind_type yytoken)
  {
    /* Establish the initial context for the current lookahead if no initial
       context is currently established.

       We define a context as a snapshot of the parser stacks.  We define
       the initial context for a lookahead as the context in which the
       parser initially examines that lookahead in order to select a
       syntactic action.  Thus, if the lookahead eventually proves
       syntactically unacceptable (possibly in a later context reached via a
       series of reductions), the initial context can be used to determine
       the exact set of tokens that would be syntactically acceptable in the
       lookahead's place.  Moreover, it is the context after which any
       further semantic actions would be erroneous because they would be
       determined by a syntactically unacceptable token.

       yy_lac_establish_ should be invoked when a reduction is about to be
       performed in an inconsistent state (which, for the purposes of LAC,
       includes consistent states that don't know they're consistent because
       their default reductions have been disabled).

       For parse.lac=full, the implementation of yy_lac_establish_ is as
       follows.  If no initial context is currently established for the
       current lookahead, then check if that lookahead can eventually be
       shifted if syntactic actions continue from the current context.  */
    if (!yy_lac_established_)
      {
#if YYDEBUG
        YYCDEBUG << "LAC: initial context established for "
                 << symbol_name (yytoken) << '\n';
#endif
        yy_lac_established_ = true;
        return yy_lac_check_ (yytoken);
      }
    return true;
  }

  // Discard any previous initial lookahead context.
  void
  parser::yy_lac_discard_ (const char* evt)
  {
   /* Discard any previous initial lookahead context because of Event,
      which may be a lookahead change or an invalidation of the currently
      established initial context for the current lookahead.

      The most common example of a lookahead change is a shift.  An example
      of both cases is syntax error recovery.  That is, a syntax error
      occurs when the lookahead is syntactically erroneous for the
      currently established initial context, so error recovery manipulates
      the parser stacks to try to find a new initial context in which the
      current lookahead is syntactically acceptable.  If it fails to find
      such a context, it discards the lookahead.  */
    if (yy_lac_established_)
      {
        YYCDEBUG << "LAC: initial context discarded due to "
                 << evt << '\n';
        yy_lac_established_ = false;
      }
  }

  int
  parser::yy_syntax_error_arguments_ (const context& yyctx,
                                                 symbol_kind_type yyarg[], int yyargn) const
  {
    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
         In the first two cases, it might appear that the current syntax
         error should have been detected in the previous state when
         yy_lac_check was invoked.  However, at that time, there might
         have been a different syntax error that discarded a different
         initial context during error recovery, leaving behind the
         current lookahead.
    */

    if (!yyctx.lookahead ().empty ())
      {
        if (yyarg)
          yyarg[0] = yyctx.token ();
        int yyn = yyctx.expected_tokens (yyarg ? yyarg + 1 : yyarg, yyargn - 1);
        return yyn + 1;
      }
    return 0;
  }

  // Generate an error message.
  std::string
  parser::yysyntax_error_ (const context& yyctx) const
  {
    // Its maximum.
    enum { YYARGS_MAX = 5 };
    // Arguments of yyformat.
    symbol_kind_type yyarg[YYARGS_MAX];
    int yycount = yy_syntax_error_arguments_ (yyctx, yyarg, YYARGS_MAX);

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
      default: // Avoid compiler warnings.
        YYCASE_ (0, YY_("syntax error"));
        YYCASE_ (1, YY_("syntax error, unexpected %s"));
        YYCASE_ (2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_ (3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_ (4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_ (5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    std::ptrdiff_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += symbol_name (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const signed char parser::yypact_ninf_ = -8;

  const signed char parser::yytable_ninf_ = -1;

  const signed char
  parser::yypact_[] =
  {
      -8,     3,     7,    -8,    21,    -8,     6,     8,    -1,    -8,
      -8,    20,    30,    -8,    31,     8,    -8,    -8,    -8,    13,
      -8,    27,    26,    11,    -8,    -8,     5,     8,    -8,    22,
       8,    -8,     8,    -8,    28,     8,    33,    37,     8,    38,
      -8,    -8,    39,    -8,    -8
  };

  const signed char
  parser::yydefact_[] =
  {
       4,     0,     2,     1,     0,     3,     0,     0,     0,     6,
       7,     0,     0,    17,     0,     0,    17,     5,    14,     0,
      19,     0,     0,     0,    12,    16,     0,     0,     8,     0,
       0,    13,     0,    18,     0,     0,     0,     0,     0,     0,
       9,    11,     0,    15,    10
  };

  const signed char
  parser::yypgoto_[] =
  {
      -8,    -8,    -8,    -8,    -7,    -8,    -8,    29,    -8
  };

  const signed char
  parser::yydefgoto_[] =
  {
       0,     1,     2,     5,    25,    23,    31,    19,    26
  };

  const signed char
  parser::yytable_[] =
  {
      11,    16,     8,     3,    12,    13,    14,    15,    21,    32,
       4,     8,     9,    10,    29,    30,     8,    24,    33,     7,
      34,     9,    10,    36,    17,    37,     9,    10,    39,     8,
      28,    42,     6,    18,    20,    35,    27,    40,    38,     9,
      10,    41,    43,    44,     0,    22
  };

  const signed char
  parser::yycheck_[] =
  {
       7,     8,     3,     0,     5,     6,     7,     8,    15,     4,
       3,     3,    13,    14,     3,     4,     3,     4,    13,    13,
      27,    13,    14,    30,     4,    32,    13,    14,    35,     3,
       4,    38,    11,     3,     3,    13,     9,     4,    10,    13,
      14,     4,     4,     4,    -1,    16
  };

  const signed char
  parser::yystos_[] =
  {
       0,    16,    17,     0,     3,    18,    11,    13,     3,    13,
      14,    19,     5,     6,     7,     8,    19,     4,     3,    22,
       3,    19,    22,    20,     4,    19,    23,     9,     4,     3,
       4,    21,     4,    13,    19,    13,    19,    19,    10,    19,
       4,     4,    19,     4,     4
  };

  const signed char
  parser::yyr1_[] =
  {
       0,    15,    16,    17,    17,    18,    19,    19,    19,    19,
      19,    19,    19,    20,    20,    21,    22,    22,    23,    23
  };

  const signed char
  parser::yyr2_[] =
  {
       0,     2,     1,     2,     0,     5,     1,     1,     4,     7,
       8,     7,     4,     2,     0,     4,     2,     0,     2,     0
  };




#if YYDEBUG
  const signed char
  parser::yyrline_[] =
  {
       0,    44,    44,    46,    47,    48,    50,    51,    52,    53,
      54,    55,    56,    58,    59,    61,    63,    64,    66,    67
  };

  void
  parser::yy_stack_print_ () const
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  void
  parser::yy_reduce_print_ (int yyrule) const
  {
    int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG


} // yy
#line 1392 "src/parser.cpp"

