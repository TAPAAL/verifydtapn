
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison LALR(1) parsers in C++
   
      Copyright (C) 2002, 2003, 2004, 2005, 2006, 2007, 2008 Free Software
   Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* First part of user declarations.  */


/* Line 311 of lalr1.cc  */
#line 41 "Core/QueryParser/Generated/parser.cpp"


#include "parser.hpp"

/* User implementation prologue.  */


/* Line 317 of lalr1.cc  */
#line 50 "Core/QueryParser/Generated/parser.cpp"
/* Unqualified %code blocks.  */

/* Line 318 of lalr1.cc  */
#line 38 "Core/QueryParser/grammar.yy"

	#include "../TAPNQueryParser.hpp"



/* Line 318 of lalr1.cc  */
#line 61 "Core/QueryParser/Generated/parser.cpp"

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* FIXME: INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#define YYUSE(e) ((void) (e))

/* Enable debugging if requested.  */
#if YYDEBUG

/* A pseudo ostream that takes yydebug_ into account.  */
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)	\
do {							\
  if (yydebug_)						\
    {							\
      *yycdebug_ << Title << ' ';			\
      yy_symbol_print_ ((Type), (Value), (Location));	\
      *yycdebug_ << std::endl;				\
    }							\
} while (false)

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug_)				\
    yy_reduce_print_ (Rule);		\
} while (false)

# define YY_STACK_PRINT()		\
do {					\
  if (yydebug_)				\
    yystack_print_ ();			\
} while (false)

#else /* !YYDEBUG */

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_REDUCE_PRINT(Rule)
# define YY_STACK_PRINT()

#endif /* !YYDEBUG */

#define yyerrok		(yyerrstatus_ = 0)
#define yyclearin	(yychar = yyempty_)

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)


/* Line 380 of lalr1.cc  */
#line 5 "Core/QueryParser/grammar.yy"
namespace VerifyTAPN {

/* Line 380 of lalr1.cc  */
#line 129 "Core/QueryParser/Generated/parser.cpp"
#if YYERROR_VERBOSE

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
              /* Fall through.  */
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

#endif

  /// Build a parser object.
  Parser::Parser (VerifyTAPN::TAPNQueryParser& driver_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      driver (driver_yyarg)
  {
  }

  Parser::~Parser ()
  {
  }

#if YYDEBUG
  /*--------------------------------.
  | Print this symbol on YYOUTPUT.  |
  `--------------------------------*/

  inline void
  Parser::yy_symbol_value_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yyvaluep);
    switch (yytype)
      {
         default:
	  break;
      }
  }


  void
  Parser::yy_symbol_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    *yycdebug_ << (yytype < yyntokens_ ? "token" : "nterm")
	       << ' ' << yytname_[yytype] << " ("
	       << *yylocationp << ": ";
    yy_symbol_value_print_ (yytype, yyvaluep, yylocationp);
    *yycdebug_ << ')';
  }
#endif

  void
  Parser::yydestruct_ (const char* yymsg,
			   int yytype, semantic_type* yyvaluep, location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yymsg);
    YYUSE (yyvaluep);

    YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

    switch (yytype)
      {
        case 3: /* "IDENTIFIER" */

/* Line 480 of lalr1.cc  */
#line 56 "Core/QueryParser/grammar.yy"
	{ delete (yyvaluep->string); };

/* Line 480 of lalr1.cc  */
#line 236 "Core/QueryParser/Generated/parser.cpp"
	break;
      case 4: /* "LESS" */

/* Line 480 of lalr1.cc  */
#line 56 "Core/QueryParser/grammar.yy"
	{ delete (yyvaluep->string); };

/* Line 480 of lalr1.cc  */
#line 245 "Core/QueryParser/Generated/parser.cpp"
	break;
      case 5: /* "LESSEQUAL" */

/* Line 480 of lalr1.cc  */
#line 56 "Core/QueryParser/grammar.yy"
	{ delete (yyvaluep->string); };

/* Line 480 of lalr1.cc  */
#line 254 "Core/QueryParser/Generated/parser.cpp"
	break;
      case 6: /* "EQUAL" */

/* Line 480 of lalr1.cc  */
#line 56 "Core/QueryParser/grammar.yy"
	{ delete (yyvaluep->string); };

/* Line 480 of lalr1.cc  */
#line 263 "Core/QueryParser/Generated/parser.cpp"
	break;
      case 7: /* "GREATEREQUAL" */

/* Line 480 of lalr1.cc  */
#line 56 "Core/QueryParser/grammar.yy"
	{ delete (yyvaluep->string); };

/* Line 480 of lalr1.cc  */
#line 272 "Core/QueryParser/Generated/parser.cpp"
	break;
      case 8: /* "GREATER" */

/* Line 480 of lalr1.cc  */
#line 56 "Core/QueryParser/grammar.yy"
	{ delete (yyvaluep->string); };

/* Line 480 of lalr1.cc  */
#line 281 "Core/QueryParser/Generated/parser.cpp"
	break;
      case 26: /* "query" */

/* Line 480 of lalr1.cc  */
#line 58 "Core/QueryParser/grammar.yy"
	{ delete (yyvaluep->query); };

/* Line 480 of lalr1.cc  */
#line 290 "Core/QueryParser/Generated/parser.cpp"
	break;
      case 27: /* "expression" */

/* Line 480 of lalr1.cc  */
#line 57 "Core/QueryParser/grammar.yy"
	{ delete (yyvaluep->expr); };

/* Line 480 of lalr1.cc  */
#line 299 "Core/QueryParser/Generated/parser.cpp"
	break;
      case 28: /* "arithmeticExpression" */

/* Line 480 of lalr1.cc  */
#line 57 "Core/QueryParser/grammar.yy"
	{ delete (yyvaluep->arexpr); };

/* Line 480 of lalr1.cc  */
#line 308 "Core/QueryParser/Generated/parser.cpp"
	break;
      case 29: /* "multiplyExpression" */

/* Line 480 of lalr1.cc  */
#line 57 "Core/QueryParser/grammar.yy"
	{ delete (yyvaluep->arexpr); };

/* Line 480 of lalr1.cc  */
#line 317 "Core/QueryParser/Generated/parser.cpp"
	break;
      case 30: /* "arithmeticParantheses" */

/* Line 480 of lalr1.cc  */
#line 57 "Core/QueryParser/grammar.yy"
	{ delete (yyvaluep->arexpr); };

/* Line 480 of lalr1.cc  */
#line 326 "Core/QueryParser/Generated/parser.cpp"
	break;
      case 31: /* "parExpression" */

/* Line 480 of lalr1.cc  */
#line 57 "Core/QueryParser/grammar.yy"
	{ delete (yyvaluep->expr); };

/* Line 480 of lalr1.cc  */
#line 335 "Core/QueryParser/Generated/parser.cpp"
	break;
      case 32: /* "notExpression" */

/* Line 480 of lalr1.cc  */
#line 57 "Core/QueryParser/grammar.yy"
	{ delete (yyvaluep->expr); };

/* Line 480 of lalr1.cc  */
#line 344 "Core/QueryParser/Generated/parser.cpp"
	break;
      case 33: /* "orExpression" */

/* Line 480 of lalr1.cc  */
#line 57 "Core/QueryParser/grammar.yy"
	{ delete (yyvaluep->expr); };

/* Line 480 of lalr1.cc  */
#line 353 "Core/QueryParser/Generated/parser.cpp"
	break;
      case 34: /* "andExpression" */

/* Line 480 of lalr1.cc  */
#line 57 "Core/QueryParser/grammar.yy"
	{ delete (yyvaluep->expr); };

/* Line 480 of lalr1.cc  */
#line 362 "Core/QueryParser/Generated/parser.cpp"
	break;
      case 35: /* "boolExpression" */

/* Line 480 of lalr1.cc  */
#line 57 "Core/QueryParser/grammar.yy"
	{ delete (yyvaluep->expr); };

/* Line 480 of lalr1.cc  */
#line 371 "Core/QueryParser/Generated/parser.cpp"
	break;
      case 36: /* "atomicProposition" */

/* Line 480 of lalr1.cc  */
#line 57 "Core/QueryParser/grammar.yy"
	{ delete (yyvaluep->expr); };

/* Line 480 of lalr1.cc  */
#line 380 "Core/QueryParser/Generated/parser.cpp"
	break;
      case 37: /* "compareOp" */

/* Line 480 of lalr1.cc  */
#line 56 "Core/QueryParser/grammar.yy"
	{ delete (yyvaluep->string); };

/* Line 480 of lalr1.cc  */
#line 389 "Core/QueryParser/Generated/parser.cpp"
	break;

	default:
	  break;
      }
  }

  void
  Parser::yypop_ (unsigned int n)
  {
    yystate_stack_.pop (n);
    yysemantic_stack_.pop (n);
    yylocation_stack_.pop (n);
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
#endif

  int
  Parser::parse ()
  {
    /// Lookahead and lookahead in internal form.
    int yychar = yyempty_;
    int yytoken = 0;

    /* State.  */
    int yyn;
    int yylen = 0;
    int yystate = 0;

    /* Error handling.  */
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// Semantic value of the lookahead.
    semantic_type yylval;
    /// Location of the lookahead.
    location_type yylloc;
    /// The locations where the error started and ended.
    location_type yyerror_range[2];

    /// $$.
    semantic_type yyval;
    /// @$.
    location_type yyloc;

    int yyresult;

    YYCDEBUG << "Starting parse" << std::endl;


    /* User initialization code.  */
    
/* Line 559 of lalr1.cc  */
#line 21 "Core/QueryParser/grammar.yy"
{
  // Initialize the initial location.
  yylloc.begin.filename = yylloc.end.filename = &driver.file;
}

/* Line 559 of lalr1.cc  */
#line 475 "Core/QueryParser/Generated/parser.cpp"

    /* Initialize the stacks.  The initial state will be pushed in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystate_stack_ = state_stack_type (0);
    yysemantic_stack_ = semantic_stack_type (0);
    yylocation_stack_ = location_stack_type (0);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* New state.  */
  yynewstate:
    yystate_stack_.push (yystate);
    YYCDEBUG << "Entering state " << yystate << std::endl;

    /* Accept?  */
    if (yystate == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    /* Backup.  */
  yybackup:

    /* Try to take a decision without lookahead.  */
    yyn = yypact_[yystate];
    if (yyn == yypact_ninf_)
      goto yydefault;

    /* Read a lookahead token.  */
    if (yychar == yyempty_)
      {
	YYCDEBUG << "Reading a token: ";
	yychar = yylex (&yylval, &yylloc, driver);
      }


    /* Convert token to internal form.  */
    if (yychar <= yyeof_)
      {
	yychar = yytoken = yyeof_;
	YYCDEBUG << "Now at end of input." << std::endl;
      }
    else
      {
	yytoken = yytranslate_ (yychar);
	YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
      }

    /* If the proper action on seeing token YYTOKEN is to reduce or to
       detect an error, take that action.  */
    yyn += yytoken;
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yytoken)
      goto yydefault;

    /* Reduce or error.  */
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
	if (yyn == 0 || yyn == yytable_ninf_)
	goto yyerrlab;
	yyn = -yyn;
	goto yyreduce;
      }

    /* Shift the lookahead token.  */
    YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

    /* Discard the token being shifted.  */
    yychar = yyempty_;

    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* Count tokens shifted since error; after three, turn off error
       status.  */
    if (yyerrstatus_)
      --yyerrstatus_;

    yystate = yyn;
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystate];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    /* If YYLEN is nonzero, implement the default value of the action:
       `$$ = $1'.  Otherwise, use the top of the stack.

       Otherwise, the following line sets YYVAL to garbage.
       This behavior is undocumented and Bison
       users should not rely upon it.  */
    if (yylen)
      yyval = yysemantic_stack_[yylen - 1];
    else
      yyval = yysemantic_stack_[0];

    {
      slice<location_type, location_stack_type> slice (yylocation_stack_, yylen);
      YYLLOC_DEFAULT (yyloc, slice, yylen);
    }
    YY_REDUCE_PRINT (yyn);
    switch (yyn)
      {
	  case 2:

/* Line 678 of lalr1.cc  */
#line 62 "Core/QueryParser/grammar.yy"
    { (yyval.query) = new VerifyTAPN::AST::Query(VerifyTAPN::AST::EF, (yysemantic_stack_[(2) - (2)].expr)); driver.setAST((yyval.query)); }
    break;

  case 3:

/* Line 678 of lalr1.cc  */
#line 63 "Core/QueryParser/grammar.yy"
    { (yyval.query) = new VerifyTAPN::AST::Query(VerifyTAPN::AST::AG, (yysemantic_stack_[(2) - (2)].expr)); driver.setAST((yyval.query)); }
    break;

  case 4:

/* Line 678 of lalr1.cc  */
#line 64 "Core/QueryParser/grammar.yy"
    { (yyval.query) = new VerifyTAPN::AST::Query(VerifyTAPN::AST::EG, (yysemantic_stack_[(2) - (2)].expr)); driver.setAST((yyval.query)); }
    break;

  case 5:

/* Line 678 of lalr1.cc  */
#line 65 "Core/QueryParser/grammar.yy"
    { (yyval.query) = new VerifyTAPN::AST::Query(VerifyTAPN::AST::AF, (yysemantic_stack_[(2) - (2)].expr)); driver.setAST((yyval.query)); }
    break;

  case 6:

/* Line 678 of lalr1.cc  */
#line 68 "Core/QueryParser/grammar.yy"
    { (yyval.expr) = (yysemantic_stack_[(1) - (1)].expr); }
    break;

  case 7:

/* Line 678 of lalr1.cc  */
#line 69 "Core/QueryParser/grammar.yy"
    { (yyval.expr) = (yysemantic_stack_[(1) - (1)].expr); }
    break;

  case 8:

/* Line 678 of lalr1.cc  */
#line 70 "Core/QueryParser/grammar.yy"
    { (yyval.expr) = (yysemantic_stack_[(1) - (1)].expr); }
    break;

  case 9:

/* Line 678 of lalr1.cc  */
#line 71 "Core/QueryParser/grammar.yy"
    { (yyval.expr) = (yysemantic_stack_[(1) - (1)].expr); }
    break;

  case 10:

/* Line 678 of lalr1.cc  */
#line 72 "Core/QueryParser/grammar.yy"
    { (yyval.expr) = (yysemantic_stack_[(1) - (1)].expr); }
    break;

  case 11:

/* Line 678 of lalr1.cc  */
#line 73 "Core/QueryParser/grammar.yy"
    { (yyval.expr) = (yysemantic_stack_[(1) - (1)].expr); }
    break;

  case 12:

/* Line 678 of lalr1.cc  */
#line 79 "Core/QueryParser/grammar.yy"
    { (yyval.arexpr) = new VerifyTAPN::AST::PlusExpression((yysemantic_stack_[(3) - (1)].arexpr), (yysemantic_stack_[(3) - (3)].arexpr)); }
    break;

  case 13:

/* Line 678 of lalr1.cc  */
#line 80 "Core/QueryParser/grammar.yy"
    { (yyval.arexpr) = new VerifyTAPN::AST::SubtractExpression((yysemantic_stack_[(3) - (1)].arexpr), (yysemantic_stack_[(3) - (3)].arexpr)); }
    break;

  case 14:

/* Line 678 of lalr1.cc  */
#line 81 "Core/QueryParser/grammar.yy"
    { (yyval.arexpr) = new VerifyTAPN::AST::MinusExpression((yysemantic_stack_[(2) - (2)].arexpr)); }
    break;

  case 15:

/* Line 678 of lalr1.cc  */
#line 82 "Core/QueryParser/grammar.yy"
    { (yyval.arexpr) = (yysemantic_stack_[(1) - (1)].arexpr); }
    break;

  case 16:

/* Line 678 of lalr1.cc  */
#line 85 "Core/QueryParser/grammar.yy"
    { (yyval.arexpr) = new VerifyTAPN::AST::MultiplyExpression((yysemantic_stack_[(3) - (1)].arexpr), (yysemantic_stack_[(3) - (3)].arexpr)); }
    break;

  case 17:

/* Line 678 of lalr1.cc  */
#line 86 "Core/QueryParser/grammar.yy"
    { (yyval.arexpr) = (yysemantic_stack_[(1) - (1)].arexpr); }
    break;

  case 18:

/* Line 678 of lalr1.cc  */
#line 89 "Core/QueryParser/grammar.yy"
    { (yyval.arexpr) = (yysemantic_stack_[(3) - (2)].arexpr); }
    break;

  case 19:

/* Line 678 of lalr1.cc  */
#line 90 "Core/QueryParser/grammar.yy"
    { (yyval.arexpr) = new VerifyTAPN::AST::NumberExpression((yysemantic_stack_[(1) - (1)].number));}
    break;

  case 20:

/* Line 678 of lalr1.cc  */
#line 91 "Core/QueryParser/grammar.yy"
    { 
                                                                    int placeIndex = driver.getTAPN().getPlaceIndex(*(yysemantic_stack_[(1) - (1)].string));
                                                                    delete (yysemantic_stack_[(1) - (1)].string);
                                                                    if(placeIndex == -1) error((yylocation_stack_[(1) - (1)]), "unknown place"); 
                                                                    (yyval.arexpr) = new VerifyTAPN::AST::IdentifierExpression(placeIndex); 
                                                                 }
    break;

  case 21:

/* Line 678 of lalr1.cc  */
#line 99 "Core/QueryParser/grammar.yy"
    { (yyval.expr) = (yysemantic_stack_[(3) - (2)].expr); }
    break;

  case 22:

/* Line 678 of lalr1.cc  */
#line 100 "Core/QueryParser/grammar.yy"
    { (yyval.expr) = new VerifyTAPN::AST::NotExpression((yysemantic_stack_[(2) - (2)].expr)); }
    break;

  case 23:

/* Line 678 of lalr1.cc  */
#line 101 "Core/QueryParser/grammar.yy"
    { (yyval.expr) = new VerifyTAPN::AST::OrExpression((yysemantic_stack_[(3) - (1)].expr), (yysemantic_stack_[(3) - (3)].expr)); }
    break;

  case 24:

/* Line 678 of lalr1.cc  */
#line 102 "Core/QueryParser/grammar.yy"
    { (yyval.expr) = new VerifyTAPN::AST::AndExpression((yysemantic_stack_[(3) - (1)].expr), (yysemantic_stack_[(3) - (3)].expr)); }
    break;

  case 25:

/* Line 678 of lalr1.cc  */
#line 103 "Core/QueryParser/grammar.yy"
    { (yyval.expr) = new VerifyTAPN::AST::BoolExpression(true); }
    break;

  case 26:

/* Line 678 of lalr1.cc  */
#line 104 "Core/QueryParser/grammar.yy"
    { (yyval.expr) = new VerifyTAPN::AST::BoolExpression(false); }
    break;

  case 27:

/* Line 678 of lalr1.cc  */
#line 106 "Core/QueryParser/grammar.yy"
    { (yyval.expr) = new VerifyTAPN::AST::AtomicProposition((yysemantic_stack_[(3) - (1)].arexpr), (yysemantic_stack_[(3) - (2)].string), (yysemantic_stack_[(3) - (3)].arexpr)); }
    break;

  case 28:

/* Line 678 of lalr1.cc  */
#line 107 "Core/QueryParser/grammar.yy"
    { (yyval.expr) = new VerifyTAPN::AST::DeadlockExpression(); }
    break;



/* Line 678 of lalr1.cc  */
#line 788 "Core/QueryParser/Generated/parser.cpp"
	default:
          break;
      }
    YY_SYMBOL_PRINT ("-> $$ =", yyr1_[yyn], &yyval, &yyloc);

    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();

    yysemantic_stack_.push (yyval);
    yylocation_stack_.push (yyloc);

    /* Shift the result of the reduction.  */
    yyn = yyr1_[yyn];
    yystate = yypgoto_[yyn - yyntokens_] + yystate_stack_[0];
    if (0 <= yystate && yystate <= yylast_
	&& yycheck_[yystate] == yystate_stack_[0])
      yystate = yytable_[yystate];
    else
      yystate = yydefgoto_[yyn - yyntokens_];
    goto yynewstate;

  /*------------------------------------.
  | yyerrlab -- here on detecting error |
  `------------------------------------*/
  yyerrlab:
    /* If not already recovering from an error, report this error.  */
    if (!yyerrstatus_)
      {
	++yynerrs_;
	error (yylloc, yysyntax_error_ (yystate, yytoken));
      }

    yyerror_range[0] = yylloc;
    if (yyerrstatus_ == 3)
      {
	/* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

	if (yychar <= yyeof_)
	  {
	  /* Return failure if at end of input.  */
	  if (yychar == yyeof_)
	    YYABORT;
	  }
	else
	  {
	    yydestruct_ ("Error: discarding", yytoken, &yylval, &yylloc);
	    yychar = yyempty_;
	  }
      }

    /* Else will try to reuse lookahead token after shifting the error
       token.  */
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

    yyerror_range[0] = yylocation_stack_[yylen - 1];
    /* Do not reclaim the symbols of the rule which action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    yystate = yystate_stack_[0];
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;	/* Each real token shifted decrements this.  */

    for (;;)
      {
	yyn = yypact_[yystate];
	if (yyn != yypact_ninf_)
	{
	  yyn += yyterror_;
	  if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
	    {
	      yyn = yytable_[yyn];
	      if (0 < yyn)
		break;
	    }
	}

	/* Pop the current state because it cannot handle the error token.  */
	if (yystate_stack_.height () == 1)
	YYABORT;

	yyerror_range[0] = yylocation_stack_[0];
	yydestruct_ ("Error: popping",
		     yystos_[yystate],
		     &yysemantic_stack_[0], &yylocation_stack_[0]);
	yypop_ ();
	yystate = yystate_stack_[0];
	YY_STACK_PRINT ();
      }

    yyerror_range[1] = yylloc;
    // Using YYLLOC is tempting, but would change the location of
    // the lookahead.  YYLOC is available though.
    YYLLOC_DEFAULT (yyloc, (yyerror_range - 1), 2);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yyloc);

    /* Shift the error token.  */
    YY_SYMBOL_PRINT ("Shifting", yystos_[yyn],
		     &yysemantic_stack_[0], &yylocation_stack_[0]);

    yystate = yyn;
    goto yynewstate;

    /* Accept.  */
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    /* Abort.  */
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (yychar != yyempty_)
      yydestruct_ ("Cleanup: discarding lookahead", yytoken, &yylval, &yylloc);

    /* Do not reclaim the symbols of the rule which action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (yystate_stack_.height () != 1)
      {
	yydestruct_ ("Cleanup: popping",
		   yystos_[yystate_stack_[0]],
		   &yysemantic_stack_[0],
		   &yylocation_stack_[0]);
	yypop_ ();
      }

    return yyresult;
  }

  // Generate an error message.
  std::string
  Parser::yysyntax_error_ (int yystate, int tok)
  {
    std::string res;
    YYUSE (yystate);
#if YYERROR_VERBOSE
    int yyn = yypact_[yystate];
    if (yypact_ninf_ < yyn && yyn <= yylast_)
      {
	/* Start YYX at -YYN if negative to avoid negative indexes in
	   YYCHECK.  */
	int yyxbegin = yyn < 0 ? -yyn : 0;

	/* Stay within bounds of both yycheck and yytname.  */
	int yychecklim = yylast_ - yyn + 1;
	int yyxend = yychecklim < yyntokens_ ? yychecklim : yyntokens_;
	int count = 0;
	for (int x = yyxbegin; x < yyxend; ++x)
	  if (yycheck_[x + yyn] == x && x != yyterror_)
	    ++count;

	// FIXME: This method of building the message is not compatible
	// with internationalization.  It should work like yacc.c does it.
	// That is, first build a string that looks like this:
	// "syntax error, unexpected %s or %s or %s"
	// Then, invoke YY_ on this string.
	// Finally, use the string as a format to output
	// yytname_[tok], etc.
	// Until this gets fixed, this message appears in English only.
	res = "syntax error, unexpected ";
	res += yytnamerr_ (yytname_[tok]);
	if (count < 5)
	  {
	    count = 0;
	    for (int x = yyxbegin; x < yyxend; ++x)
	      if (yycheck_[x + yyn] == x && x != yyterror_)
		{
		  res += (!count++) ? ", expecting " : " or ";
		  res += yytnamerr_ (yytname_[x]);
		}
	  }
      }
    else
#endif
      res = YY_("syntax error");
    return res;
  }


  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
  const signed char Parser::yypact_ninf_ = -29;
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

  /* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
     doesn't specify something else to do.  Zero means the default is an
     error.  */
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

  /* YYPGOTO[NTERM-NUM].  */
  const signed char
  Parser::yypgoto_[] =
  {
       -29,   -29,     2,    -8,   -12,   -28,    22,   -29,   -29,   -29,
     -29,   -29,   -29
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const signed char
  Parser::yydefgoto_[] =
  {
        -1,     5,    28,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    43
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If zero, do what YYDEFACT says.  */
  const signed char Parser::yytable_ninf_ = -1;
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

  /* YYCHECK.  */
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

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
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

#if YYDEBUG
  /* TOKEN_NUMBER_[YYLEX-NUM] -- Internal symbol number corresponding
     to YYLEX-NUM.  */
  const unsigned short int
  Parser::yytoken_number_[] =
  {
         0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  Parser::yyr1_[] =
  {
         0,    25,    26,    26,    26,    26,    27,    27,    27,    27,
      27,    27,    28,    28,    28,    28,    29,    29,    30,    30,
      30,    31,    32,    33,    34,    35,    35,    36,    36,    37,
      37,    37,    37,    37
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
  const unsigned char
  Parser::yyr2_[] =
  {
         0,     2,     2,     2,     2,     2,     1,     1,     1,     1,
       1,     1,     3,     3,     2,     1,     3,     1,     3,     1,
       1,     3,     2,     3,     3,     1,     1,     3,     1,     1,
       1,     1,     1,     1
  };

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
  /* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
     First, the terminals, then, starting at \a yyntokens_, nonterminals.  */
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
  "compareOp", 0
  };
#endif

#if YYDEBUG
  /* YYRHS -- A `-1'-separated list of the rules' RHS.  */
  const Parser::rhs_number_type
  Parser::yyrhs_[] =
  {
        26,     0,    -1,    10,    27,    -1,    11,    27,    -1,    13,
      27,    -1,    12,    27,    -1,    31,    -1,    32,    -1,    33,
      -1,    34,    -1,    36,    -1,    35,    -1,    28,    22,    29,
      -1,    28,    23,    29,    -1,    23,    29,    -1,    29,    -1,
      29,    24,    30,    -1,    30,    -1,    14,    28,    15,    -1,
       9,    -1,     3,    -1,    14,    27,    15,    -1,    18,    31,
      -1,    27,    16,    27,    -1,    27,    17,    27,    -1,    19,
      -1,    20,    -1,    28,    37,    28,    -1,    21,    -1,     4,
      -1,     5,    -1,     6,    -1,     7,    -1,     8,    -1
  };

  /* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
     YYRHS.  */
  const unsigned char
  Parser::yyprhs_[] =
  {
         0,     0,     3,     6,     9,    12,    15,    17,    19,    21,
      23,    25,    27,    31,    35,    38,    40,    44,    46,    50,
      52,    54,    58,    61,    65,    69,    71,    73,    77,    79,
      81,    83,    85,    87
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
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
    for (state_stack_type::const_iterator i = yystate_stack_.begin ();
	 i != yystate_stack_.end (); ++i)
      *yycdebug_ << ' ' << *i;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  Parser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    /* Print the symbols being reduced, and their result.  */
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
	       << " (line " << yylno << "):" << std::endl;
    /* The symbols being reduced.  */
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
		       yyrhs_[yyprhs_[yyrule] + yyi],
		       &(yysemantic_stack_[(yynrhs) - (yyi + 1)]),
		       &(yylocation_stack_[(yynrhs) - (yyi + 1)]));
  }
#endif // YYDEBUG

  /* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
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
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int Parser::yyeof_ = 0;
  const int Parser::yylast_ = 66;
  const int Parser::yynnts_ = 13;
  const int Parser::yyempty_ = -2;
  const int Parser::yyfinal_ = 27;
  const int Parser::yyterror_ = 1;
  const int Parser::yyerrcode_ = 256;
  const int Parser::yyntokens_ = 25;

  const unsigned int Parser::yyuser_token_number_max_ = 279;
  const Parser::token_number_type Parser::yyundef_token_ = 2;


/* Line 1054 of lalr1.cc  */
#line 5 "Core/QueryParser/grammar.yy"
} // VerifyTAPN

/* Line 1054 of lalr1.cc  */
#line 1253 "Core/QueryParser/Generated/parser.cpp"


/* Line 1056 of lalr1.cc  */
#line 111 "Core/QueryParser/grammar.yy"


void 
VerifyTAPN::Parser::error (const VerifyTAPN::Parser::location_type& l,
                        const std::string& m)
{
	driver.error (l, m);
	exit(1);
}

