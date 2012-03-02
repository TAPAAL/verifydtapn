%{                                            /* -*- C++ -*- */
# include <string>
# include "../TAPNQueryParser.hpp"
# include "parser.hpp"
 
/* Work around an incompatibility in flex (at least versions
   2.5.31 through 2.5.33): it generates code that does
   not conform to C89.  See Debian bug 333231
   <http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=333231>.  */
# undef yywrap
# define yywrap() 1
 
/* By default yylex returns int, we use token_type.
   Unfortunately yyterminate by default returns 0, which is
   not of token_type.  */
#define yyterminate() return token::END
%}

%option noyywrap nounput batch

id    [a-zA-Z][a-zA-Z_0-9]*
int   [0-9]+
blank [ \t]

%{
# define YY_USER_ACTION  yylloc->columns (yyleng);
%}
%%
%{
  yylloc->step ();
%}
[ \t]+  					{ yylloc->step (); }
[\n]+       				{ yylloc->lines (yyleng); yylloc->step (); }

%{
  typedef VerifyTAPN::Parser::token token;
%}

[0-9]+						{ yylval->number = atoi(yytext); return token::NUMBER; }
^EF							{ return token::EF; }
^AG							{ return token::AG; }
or|\|\|						{ return token::OR; }
and|&&						{ return token::AND; }
true						{ return token::BOOL_TRUE; }
false						{ return token::BOOL_FALSE; }
"!"|not						{ return token::NOT; }
"("							{ return token::LPARAN; }
")"							{ return token::RPARAN; }
"<"							{ yylval->string = new std::string(yytext); return token::LESS; }
"<="						{ yylval->string = new std::string(yytext); return token::LESSEQUAL; }
"="							{ yylval->string = new std::string(yytext); return token::EQUAL; }
"=="						{ yylval->string = new std::string(yytext); return token::EQUAL; }
">="						{ yylval->string = new std::string(yytext); return token::GREATEREQUAL; }
">"							{ yylval->string = new std::string(yytext); return token::GREATER; }
[a-zA-Z_][a-zA-Z_0-9]*      { yylval->string = new std::string(yytext); return token::IDENTIFIER; }

%%
namespace VerifyTAPN
{
	void TAPNQueryParser::scan_begin ()
	{
		if (!(yyin = fopen (file.c_str (), "r")))
		{
			error (std::string ("cannot open ") + file);
			std::cout << errno;
			exit (1);
		}
	}
	     
	void TAPNQueryParser::scan_end ()
	{
		fclose (yyin);
	}
}

