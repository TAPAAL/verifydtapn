// A Bison parser, made by GNU Bison 3.0.2.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2013 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

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


// First part of user declarations.

#line 37 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:399

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

#include "parser.hpp"

// User implementation prologue.

#line 51 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:407
// Unqualified %code blocks.
#line 38 "Core/QueryParser/grammar.yy" // lalr1.cc:408

	#include "../TAPNQueryParser.hpp"

#line 57 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:408


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
    while (/*CONSTCOND*/ false)
# endif


// Suppress unused-variable warnings by "using" E.
#define YYUSE(E) ((void) (E))

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
      *yycdebug_ << std::endl;                  \
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
      yystack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YYUSE(Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void>(0)
# define YY_STACK_PRINT()                static_cast<void>(0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyempty = true)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

#line 5 "Core/QueryParser/grammar.yy" // lalr1.cc:474
namespace VerifyTAPN {
#line 143 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:474

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  Parser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr = "";
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              // Fall through.
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }


  /// Build a parser object.
  Parser::Parser (VerifyTAPN::TAPNQueryParser& driver_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      driver (driver_yyarg)
  {}

  Parser::~Parser ()
  {}


  /*---------------.
  | Symbol types.  |
  `---------------*/

  inline
  Parser::syntax_error::syntax_error (const location_type& l, const std::string& m)
    : std::runtime_error (m)
    , location (l)
  {}

  // basic_symbol.
  template <typename Base>
  inline
  Parser::basic_symbol<Base>::basic_symbol ()
    : value ()
  {}

  template <typename Base>
  inline
  Parser::basic_symbol<Base>::basic_symbol (const basic_symbol& other)
    : Base (other)
    , value ()
    , location (other.location)
  {
    value = other.value;
  }


  template <typename Base>
  inline
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const semantic_type& v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}


  /// Constructor for valueless symbols.
  template <typename Base>
  inline
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const location_type& l)
    : Base (t)
    , value ()
    , location (l)
  {}

  template <typename Base>
  inline
  Parser::basic_symbol<Base>::~basic_symbol ()
  {
  }

  template <typename Base>
  inline
  void
  Parser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move(s);
    value = s.value;
    location = s.location;
  }

  // by_type.
  inline
  Parser::by_type::by_type ()
     : type (empty)
  {}

  inline
  Parser::by_type::by_type (const by_type& other)
    : type (other.type)
  {}

  inline
  Parser::by_type::by_type (token_type t)
    : type (yytranslate_ (t))
  {}

  inline
  void
  Parser::by_type::move (by_type& that)
  {
    type = that.type;
    that.type = empty;
  }

  inline
  int
  Parser::by_type::type_get () const
  {
    return type;
  }


  // by_state.
  inline
  Parser::by_state::by_state ()
    : state (empty)
  {}

  inline
  Parser::by_state::by_state (const by_state& other)
    : state (other.state)
  {}

  inline
  void
  Parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.state = empty;
  }

  inline
  Parser::by_state::by_state (state_type s)
    : state (s)
  {}

  inline
  Parser::symbol_number_type
  Parser::by_state::type_get () const
  {
    return state == empty ? 0 : yystos_[state];
  }

  inline
  Parser::stack_symbol_type::stack_symbol_type ()
  {}


  inline
  Parser::stack_symbol_type::stack_symbol_type (state_type s, symbol_type& that)
    : super_type (s, that.location)
  {
    value = that.value;
    // that is emptied.
    that.type = empty;
  }

  inline
  Parser::stack_symbol_type&
  Parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    value = that.value;
    location = that.location;
    return *this;
  }


  template <typename Base>
  inline
  void
  Parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);

    // User destructor.
    switch (yysym.type_get ())
    {
            case 3: // IDENTIFIER

#line 56 "Core/QueryParser/grammar.yy" // lalr1.cc:599
        { delete (yysym.value.string); }
#line 362 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:599
        break;

      case 4: // LESS

#line 56 "Core/QueryParser/grammar.yy" // lalr1.cc:599
        { delete (yysym.value.string); }
#line 369 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:599
        break;

      case 5: // LESSEQUAL

#line 56 "Core/QueryParser/grammar.yy" // lalr1.cc:599
        { delete (yysym.value.string); }
#line 376 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:599
        break;

      case 6: // EQUAL

#line 56 "Core/QueryParser/grammar.yy" // lalr1.cc:599
        { delete (yysym.value.string); }
#line 383 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:599
        break;

      case 7: // GREATEREQUAL

#line 56 "Core/QueryParser/grammar.yy" // lalr1.cc:599
        { delete (yysym.value.string); }
#line 390 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:599
        break;

      case 8: // GREATER

#line 56 "Core/QueryParser/grammar.yy" // lalr1.cc:599
        { delete (yysym.value.string); }
#line 397 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:599
        break;

      case 26: // query

#line 58 "Core/QueryParser/grammar.yy" // lalr1.cc:599
        { delete (yysym.value.query); }
#line 404 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:599
        break;

      case 27: // expression

#line 57 "Core/QueryParser/grammar.yy" // lalr1.cc:599
        { delete (yysym.value.expr); }
#line 411 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:599
        break;

      case 28: // arithmeticExpression

#line 57 "Core/QueryParser/grammar.yy" // lalr1.cc:599
        { delete (yysym.value.arexpr); }
#line 418 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:599
        break;

      case 29: // multiplyExpression

#line 57 "Core/QueryParser/grammar.yy" // lalr1.cc:599
        { delete (yysym.value.arexpr); }
#line 425 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:599
        break;

      case 30: // arithmeticParantheses

#line 57 "Core/QueryParser/grammar.yy" // lalr1.cc:599
        { delete (yysym.value.arexpr); }
#line 432 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:599
        break;

      case 31: // parExpression

#line 57 "Core/QueryParser/grammar.yy" // lalr1.cc:599
        { delete (yysym.value.expr); }
#line 439 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:599
        break;

      case 32: // notExpression

#line 57 "Core/QueryParser/grammar.yy" // lalr1.cc:599
        { delete (yysym.value.expr); }
#line 446 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:599
        break;

      case 33: // orExpression

#line 57 "Core/QueryParser/grammar.yy" // lalr1.cc:599
        { delete (yysym.value.expr); }
#line 453 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:599
        break;

      case 34: // andExpression

#line 57 "Core/QueryParser/grammar.yy" // lalr1.cc:599
        { delete (yysym.value.expr); }
#line 460 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:599
        break;

      case 35: // boolExpression

#line 57 "Core/QueryParser/grammar.yy" // lalr1.cc:599
        { delete (yysym.value.expr); }
#line 467 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:599
        break;

      case 36: // atomicProposition

#line 57 "Core/QueryParser/grammar.yy" // lalr1.cc:599
        { delete (yysym.value.expr); }
#line 474 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:599
        break;

      case 37: // compareOp

#line 56 "Core/QueryParser/grammar.yy" // lalr1.cc:599
        { delete (yysym.value.string); }
#line 481 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:599
        break;


      default:
        break;
    }
  }

#if YYDEBUG
  template <typename Base>
  void
  Parser::yy_print_ (std::ostream& yyo,
                                     const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    symbol_number_type yytype = yysym.type_get ();
    yyo << (yytype < yyntokens_ ? "token" : "nterm")
        << ' ' << yytname_[yytype] << " ("
        << yysym.location << ": ";
    YYUSE (yytype);
    yyo << ')';
  }
#endif

  inline
  void
  Parser::yypush_ (const char* m, state_type s, symbol_type& sym)
  {
    stack_symbol_type t (s, sym);
    yypush_ (m, t);
  }

  inline
  void
  Parser::yypush_ (const char* m, stack_symbol_type& s)
  {
    if (m)
      YY_SYMBOL_PRINT (m, s);
    yystack_.push (s);
  }

  inline
  void
  Parser::yypop_ (unsigned int n)
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  Parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  Parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  Parser::debug_level_type
  Parser::debug_level () const
  {
    return yydebug_;
  }

  void
  Parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  inline Parser::state_type
  Parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - yyntokens_] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - yyntokens_];
  }

  inline bool
  Parser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  inline bool
  Parser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  Parser::parse ()
  {
    /// Whether yyla contains a lookahead.
    bool yyempty = true;

    // State.
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

    // FIXME: This shoud be completely indented.  It is not yet to
    // avoid gratuitous conflicts when merging into the master branch.
    try
      {
    YYCDEBUG << "Starting parse" << std::endl;


    // User initialization code.
    #line 21 "Core/QueryParser/grammar.yy" // lalr1.cc:725
{
  // Initialize the initial location.
  yyla.location.begin.filename = yyla.location.end.filename = &driver.file;
}

#line 618 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:725

    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, yyla);

    // A new symbol was pushed on the stack.
  yynewstate:
    YYCDEBUG << "Entering state " << yystack_[0].state << std::endl;

    // Accept?
    if (yystack_[0].state == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    // Backup.
  yybackup:

    // Try to take a decision without lookahead.
    yyn = yypact_[yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyempty)
      {
        YYCDEBUG << "Reading a token: ";
        try
          {
            yyla.type = yytranslate_ (yylex (&yyla.value, &yyla.location, driver));
          }
        catch (const syntax_error& yyexc)
          {
            error (yyexc);
            goto yyerrlab1;
          }
        yyempty = false;
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.type_get ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.type_get ())
      goto yydefault;

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Discard the token being shifted.
    yyempty = true;

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", yyn, yyla);
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_(yystack_[yylen].state, yyr1_[yyn]);
      /* If YYLEN is nonzero, implement the default value of the
         action: '$$ = $1'.  Otherwise, use the top of the stack.

         Otherwise, the following line sets YYLHS.VALUE to garbage.
         This behavior is undocumented and Bison users should not rely
         upon it.  */
      if (yylen)
        yylhs.value = yystack_[yylen - 1].value;
      else
        yylhs.value = yystack_[0].value;

      // Compute the default @$.
      {
        slice<stack_symbol_type, stack_type> slice (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, slice, yylen);
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
      try
        {
          switch (yyn)
            {
  case 2:
#line 62 "Core/QueryParser/grammar.yy" // lalr1.cc:847
    { (yylhs.value.query) = new VerifyTAPN::AST::Query(VerifyTAPN::AST::EF, (yystack_[0].value.expr)); driver.setAST((yylhs.value.query)); }
#line 732 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:847
    break;

  case 3:
#line 63 "Core/QueryParser/grammar.yy" // lalr1.cc:847
    { (yylhs.value.query) = new VerifyTAPN::AST::Query(VerifyTAPN::AST::AG, (yystack_[0].value.expr)); driver.setAST((yylhs.value.query)); }
#line 738 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:847
    break;

  case 4:
#line 64 "Core/QueryParser/grammar.yy" // lalr1.cc:847
    { (yylhs.value.query) = new VerifyTAPN::AST::Query(VerifyTAPN::AST::EG, (yystack_[0].value.expr)); driver.setAST((yylhs.value.query)); }
#line 744 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:847
    break;

  case 5:
#line 65 "Core/QueryParser/grammar.yy" // lalr1.cc:847
    { (yylhs.value.query) = new VerifyTAPN::AST::Query(VerifyTAPN::AST::AF, (yystack_[0].value.expr)); driver.setAST((yylhs.value.query)); }
#line 750 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:847
    break;

  case 6:
#line 68 "Core/QueryParser/grammar.yy" // lalr1.cc:847
    { (yylhs.value.expr) = (yystack_[0].value.expr); }
#line 756 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:847
    break;

  case 7:
#line 69 "Core/QueryParser/grammar.yy" // lalr1.cc:847
    { (yylhs.value.expr) = (yystack_[0].value.expr); }
#line 762 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:847
    break;

  case 8:
#line 70 "Core/QueryParser/grammar.yy" // lalr1.cc:847
    { (yylhs.value.expr) = (yystack_[0].value.expr); }
#line 768 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:847
    break;

  case 9:
#line 71 "Core/QueryParser/grammar.yy" // lalr1.cc:847
    { (yylhs.value.expr) = (yystack_[0].value.expr); }
#line 774 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:847
    break;

  case 10:
#line 72 "Core/QueryParser/grammar.yy" // lalr1.cc:847
    { (yylhs.value.expr) = (yystack_[0].value.expr); }
#line 780 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:847
    break;

  case 11:
#line 73 "Core/QueryParser/grammar.yy" // lalr1.cc:847
    { (yylhs.value.expr) = (yystack_[0].value.expr); }
#line 786 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:847
    break;

  case 12:
#line 79 "Core/QueryParser/grammar.yy" // lalr1.cc:847
    { (yylhs.value.arexpr) = new VerifyTAPN::AST::PlusExpression((yystack_[2].value.arexpr), (yystack_[0].value.arexpr)); }
#line 792 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:847
    break;

  case 13:
#line 80 "Core/QueryParser/grammar.yy" // lalr1.cc:847
    { (yylhs.value.arexpr) = new VerifyTAPN::AST::SubtractExpression((yystack_[2].value.arexpr), (yystack_[0].value.arexpr)); }
#line 798 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:847
    break;

  case 14:
#line 81 "Core/QueryParser/grammar.yy" // lalr1.cc:847
    { (yylhs.value.arexpr) = new VerifyTAPN::AST::MinusExpression((yystack_[0].value.arexpr)); }
#line 804 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:847
    break;

  case 15:
#line 82 "Core/QueryParser/grammar.yy" // lalr1.cc:847
    { (yylhs.value.arexpr) = (yystack_[0].value.arexpr); }
#line 810 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:847
    break;

  case 16:
#line 85 "Core/QueryParser/grammar.yy" // lalr1.cc:847
    { (yylhs.value.arexpr) = new VerifyTAPN::AST::MultiplyExpression((yystack_[2].value.arexpr), (yystack_[0].value.arexpr)); }
#line 816 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:847
    break;

  case 17:
#line 86 "Core/QueryParser/grammar.yy" // lalr1.cc:847
    { (yylhs.value.arexpr) = (yystack_[0].value.arexpr); }
#line 822 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:847
    break;

  case 18:
#line 89 "Core/QueryParser/grammar.yy" // lalr1.cc:847
    { (yylhs.value.arexpr) = (yystack_[1].value.arexpr); }
#line 828 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:847
    break;

  case 19:
#line 90 "Core/QueryParser/grammar.yy" // lalr1.cc:847
    { (yylhs.value.arexpr) = new VerifyTAPN::AST::NumberExpression((yystack_[0].value.number));}
#line 834 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:847
    break;

  case 20:
#line 91 "Core/QueryParser/grammar.yy" // lalr1.cc:847
    { 
                                                                    int placeIndex = driver.getTAPN().getPlaceIndex(*(yystack_[0].value.string));
                                                                    delete (yystack_[0].value.string);
                                                                    if(placeIndex == -1) error(yystack_[0].location, "unknown place"); 
                                                                    (yylhs.value.arexpr) = new VerifyTAPN::AST::IdentifierExpression(placeIndex); 
                                                                 }
#line 845 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:847
    break;

  case 21:
#line 99 "Core/QueryParser/grammar.yy" // lalr1.cc:847
    { (yylhs.value.expr) = (yystack_[1].value.expr); }
#line 851 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:847
    break;

  case 22:
#line 100 "Core/QueryParser/grammar.yy" // lalr1.cc:847
    { (yylhs.value.expr) = new VerifyTAPN::AST::NotExpression((yystack_[0].value.expr)); }
#line 857 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:847
    break;

  case 23:
#line 101 "Core/QueryParser/grammar.yy" // lalr1.cc:847
    { (yylhs.value.expr) = new VerifyTAPN::AST::OrExpression((yystack_[2].value.expr), (yystack_[0].value.expr)); }
#line 863 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:847
    break;

  case 24:
#line 102 "Core/QueryParser/grammar.yy" // lalr1.cc:847
    { (yylhs.value.expr) = new VerifyTAPN::AST::AndExpression((yystack_[2].value.expr), (yystack_[0].value.expr)); }
#line 869 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:847
    break;

  case 25:
#line 103 "Core/QueryParser/grammar.yy" // lalr1.cc:847
    { (yylhs.value.expr) = new VerifyTAPN::AST::BoolExpression(true); }
#line 875 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:847
    break;

  case 26:
#line 104 "Core/QueryParser/grammar.yy" // lalr1.cc:847
    { (yylhs.value.expr) = new VerifyTAPN::AST::BoolExpression(false); }
#line 881 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:847
    break;

  case 27:
#line 106 "Core/QueryParser/grammar.yy" // lalr1.cc:847
    { (yylhs.value.expr) = new VerifyTAPN::AST::AtomicProposition((yystack_[2].value.arexpr), (yystack_[1].value.string), (yystack_[0].value.arexpr)); }
#line 887 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:847
    break;

  case 28:
#line 107 "Core/QueryParser/grammar.yy" // lalr1.cc:847
    { (yylhs.value.expr) = new VerifyTAPN::AST::DeadlockExpression(); }
#line 893 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:847
    break;


#line 897 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:847
            default:
              break;
            }
        }
      catch (const syntax_error& yyexc)
        {
          error (yyexc);
          YYERROR;
        }
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;
      YY_STACK_PRINT ();

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, yylhs);
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
        error (yyla.location, yysyntax_error_ (yystack_[0].state,
                                           yyempty ? yyempty_ : yyla.type_get ()));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.type_get () == yyeof_)
          YYABORT;
        else if (!yyempty)
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyempty = true;
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:

    /* Pacify compilers like GCC when the user code never invokes
       YYERROR and the label yyerrorlab therefore never appears in user
       code.  */
    if (false)
      goto yyerrorlab;
    yyerror_range[1].location = yystack_[yylen - 1].location;
    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    {
      stack_symbol_type error_token;
      for (;;)
        {
          yyn = yypact_[yystack_[0].state];
          if (!yy_pact_value_is_default_ (yyn))
            {
              yyn += yyterror_;
              if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
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

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = yyn;
      yypush_ ("Shifting", error_token);
    }
    goto yynewstate;

    // Accept.
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    // Abort.
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (!yyempty)
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack"
                 << std::endl;
        // Do not try to display the values of the reclaimed symbols,
        // as their printer might throw an exception.
        if (!yyempty)
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
  }

  void
  Parser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what());
  }

  // Generate an error message.
  std::string
  Parser::yysyntax_error_ (state_type yystate, symbol_number_type yytoken) const
  {
    std::string yyres;
    // Number of reported tokens (one for the "unexpected", one per
    // "expected").
    size_t yycount = 0;
    // Its maximum.
    enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
    // Arguments of yyformat.
    char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];

    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yytoken) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state
         merging (from LALR or IELR) and default reductions corrupt the
         expected token list.  However, the list is correct for
         canonical LR with one exception: it will still contain any
         token that will not be accepted due to an error action in a
         later state.
    */
    if (yytoken != yyempty_)
      {
        yyarg[yycount++] = yytname_[yytoken];
        int yyn = yypact_[yystate];
        if (!yy_pact_value_is_default_ (yyn))
          {
            /* Start YYX at -YYN if negative to avoid negative indexes in
               YYCHECK.  In other words, skip the first -YYN actions for
               this state because they are default actions.  */
            int yyxbegin = yyn < 0 ? -yyn : 0;
            // Stay within bounds of both yycheck and yytname.
            int yychecklim = yylast_ - yyn + 1;
            int yyxend = yychecklim < yyntokens_ ? yychecklim : yyntokens_;
            for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
              if (yycheck_[yyx + yyn] == yyx && yyx != yyterror_
                  && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
                {
                  if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                    {
                      yycount = 1;
                      break;
                    }
                  else
                    yyarg[yycount++] = yytname_[yyx];
                }
          }
      }

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
        YYCASE_(0, YY_("syntax error"));
        YYCASE_(1, YY_("syntax error, unexpected %s"));
        YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    // Argument number.
    size_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += yytnamerr_ (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const signed char Parser::yypact_ninf_ = -29;

  const signed char Parser::yytable_ninf_ = -1;

  const signed char
  Parser::yypact_[] =
  {
      46,    -1,    -1,    -1,    -1,     7,   -29,   -29,    -1,     9,
     -29,   -29,   -29,    25,    -2,    43,    14,   -29,   -29,   -29,
     -29,   -29,   -29,   -29,    -2,    -2,    -2,   -29,    -6,    38,
      -1,   -29,    18,    14,    -1,    -1,   -29,   -29,   -29,   -29,
     -29,    25,    25,    18,    25,   -29,   -29,    40,    -5,   -29,
      14,    14,     3,   -29
  };

  const unsigned char
  Parser::yydefact_[] =
  {
       0,     0,     0,     0,     0,     0,    20,    19,     0,     0,
      25,    26,    28,     0,     2,     0,    15,    17,     6,     7,
       8,     9,    11,    10,     3,     5,     4,     1,     0,     0,
       0,    22,     0,    14,     0,     0,    29,    30,    31,    32,
      33,     0,     0,     0,     0,    21,    18,     0,    23,    24,
      12,    13,    27,    16
  };

  const signed char
  Parser::yypgoto_[] =
  {
     -29,   -29,     2,    -8,   -12,   -28,    22,   -29,   -29,   -29,
     -29,   -29,   -29
  };

  const signed char
  Parser::yydefgoto_[] =
  {
      -1,     5,    28,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    43
  };

  const unsigned char
  Parser::yytable_[] =
  {
      29,    33,     6,    14,    24,    25,    26,    27,     7,    45,
      34,    35,    35,     8,    34,    35,    53,     9,    10,    11,
      12,     6,    13,    30,    47,    41,    42,     7,     6,    50,
      51,    31,    32,     0,     7,    52,    48,    49,    44,    32,
       0,    13,    36,    37,    38,    39,    40,    36,    37,    38,
      39,    40,     0,    46,     0,    46,     1,     2,     3,     4,
      41,    42,    41,    42,     0,    41,    42
  };

  const signed char
  Parser::yycheck_[] =
  {
       8,    13,     3,     1,     2,     3,     4,     0,     9,    15,
      16,    17,    17,    14,    16,    17,    44,    18,    19,    20,
      21,     3,    23,    14,    32,    22,    23,     9,     3,    41,
      42,     9,    14,    -1,     9,    43,    34,    35,    24,    14,
      -1,    23,     4,     5,     6,     7,     8,     4,     5,     6,
       7,     8,    -1,    15,    -1,    15,    10,    11,    12,    13,
      22,    23,    22,    23,    -1,    22,    23
  };

  const unsigned char
  Parser::yystos_[] =
  {
       0,    10,    11,    12,    13,    26,     3,     9,    14,    18,
      19,    20,    21,    23,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    27,    27,    27,     0,    27,    28,
      14,    31,    14,    29,    16,    17,     4,     5,     6,     7,
       8,    22,    23,    37,    24,    15,    15,    28,    27,    27,
      29,    29,    28,    30
  };

  const unsigned char
  Parser::yyr1_[] =
  {
       0,    25,    26,    26,    26,    26,    27,    27,    27,    27,
      27,    27,    28,    28,    28,    28,    29,    29,    30,    30,
      30,    31,    32,    33,    34,    35,    35,    36,    36,    37,
      37,    37,    37,    37
  };

  const unsigned char
  Parser::yyr2_[] =
  {
       0,     2,     2,     2,     2,     2,     1,     1,     1,     1,
       1,     1,     3,     3,     2,     1,     3,     1,     3,     1,
       1,     3,     2,     3,     3,     1,     1,     3,     1,     1,
       1,     1,     1,     1
  };



  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const Parser::yytname_[] =
  {
  "END", "error", "$undefined", "IDENTIFIER", "LESS", "LESSEQUAL",
  "EQUAL", "GREATEREQUAL", "GREATER", "NUMBER", "EF", "AG", "AF", "EG",
  "LPARAN", "RPARAN", "OR", "AND", "NOT", "BOOL_TRUE", "BOOL_FALSE",
  "DEADLOCK", "PLUS", "MINUS", "MULTIPLY", "$accept", "query",
  "expression", "arithmeticExpression", "multiplyExpression",
  "arithmeticParantheses", "parExpression", "notExpression",
  "orExpression", "andExpression", "boolExpression", "atomicProposition",
  "compareOp", YY_NULLPTR
  };

#if YYDEBUG
  const unsigned char
  Parser::yyrline_[] =
  {
       0,    62,    62,    63,    64,    65,    68,    69,    70,    71,
      72,    73,    79,    80,    81,    82,    85,    86,    89,    90,
      91,    99,   100,   101,   102,   103,   104,   106,   107,   109,
     109,   109,   109,   109
  };

  // Print the state stack on the debug stream.
  void
  Parser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << i->state;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  Parser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):" << std::endl;
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG

  // Symbol number corresponding to token number t.
  inline
  Parser::token_number_type
  Parser::yytranslate_ (int t)
  {
    static
    const token_number_type
    translate_table[] =
    {
     0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24
    };
    const unsigned int user_token_number_max_ = 279;
    const token_number_type undef_token_ = 2;

    if (static_cast<int>(t) <= yyeof_)
      return yyeof_;
    else if (static_cast<unsigned int> (t) <= user_token_number_max_)
      return translate_table[t];
    else
      return undef_token_;
  }

#line 5 "Core/QueryParser/grammar.yy" // lalr1.cc:1155
} // VerifyTAPN
#line 1352 "Core/QueryParser/Generated/parser.cpp" // lalr1.cc:1155
#line 111 "Core/QueryParser/grammar.yy" // lalr1.cc:1156


void 
VerifyTAPN::Parser::error (const VerifyTAPN::Parser::location_type& l,
                        const std::string& m)
{
	driver.error (l, m);
	exit(1);
}
