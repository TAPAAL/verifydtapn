%skeleton "lalr1.cc"                          /*  -*- C++ -*- */
%require "2.4"
%defines
%define parser_class_name "Parser"
%define namespace "VerifyTAPN"

%code requires {
# include <string>
#include "../AST.hpp"
namespace VerifyTAPN{
	class TAPNQueryParser;
}
}

// the parsing context
%parse-param { VerifyTAPN::TAPNQueryParser& driver }
%lex-param   { VerifyTAPN::TAPNQueryParser& driver }

%locations
%initial-action
{
  // Initialize the initial location.
  @$.begin.filename = @$.end.filename = &driver.file;
};

%error-verbose

// Symbols.
%union
{
	int          					number;
	std::string* 					string;
	VerifyTAPN::AST::Expression*                    expr;
	VerifyTAPN::AST::Query*                         query;
};

%code{
	#include "../TAPNQueryParser.hpp"
}

%token <string> IDENTIFIER LESS LESSEQUAL EQUAL GREATEREQUAL GREATER
%token <number> NUMBER    
%token END      0
%token EF AG AF EG
%token LPARAN RPARAN
%token OR AND NOT
%token BOOL_TRUE BOOL_FALSE
%token DEADLOCK
%token PLUS MINUS MULTIPLY
%type <expr> expression notExpression parExpression orExpression andExpression boolExpression 
%type <expr> atomicProposition arithmeticExpression multiplyExpression arithmeticParantheses
%type <query> query
%type <string> compareOp

%destructor { delete $$; } IDENTIFIER LESS LESSEQUAL EQUAL GREATEREQUAL GREATER compareOp
%destructor { delete $$; } expression notExpression parExpression orExpression andExpression boolExpression atomicProposition arithmeticExpression multiplyExpression arithmeticParantheses
%destructor { delete $$; } query

%%
%start query;
query			: EF expression { $$ = new VerifyTAPN::AST::Query(VerifyTAPN::AST::EF, $2); driver.setAST($$); }
                        | AG expression { $$ = new VerifyTAPN::AST::Query(VerifyTAPN::AST::AG, $2); driver.setAST($$); }
                        | EG expression { $$ = new VerifyTAPN::AST::Query(VerifyTAPN::AST::EG, $2); driver.setAST($$); }
                        | AF expression { $$ = new VerifyTAPN::AST::Query(VerifyTAPN::AST::AF, $2); driver.setAST($$); }
                        ;

expression              : parExpression             { $$ = $1; }
                        | notExpression     { $$ = $1; }
                        | orExpression      { $$ = $1; }
                        | andExpression     { $$ = $1; }
                        | atomicProposition { $$ = $1; }
                        | boolExpression    { $$ = $1; }
                        ;

%left OR;
%left AND;

arithmeticExpression	: arithmeticExpression PLUS multiplyExpression	{ $$ = new PlusExpression($1, $3); }
                        | arithmeticExpression MINUS multiplyExpression	{ $$ = new SubtractExpression($1, $3); }
                        | MINUS multiplyExpression                    { $$ = new MinusExpression($2); }
                        | multiplyExpression                            { $$ = $1; }
                        ;

multiplyExpression	: multiplyExpression MULTIPLY arithmeticParantheses	{ $$ = new MultiplyExpression($1, $3); }
                        | arithmeticParantheses                                 { $$ = $1; }
                        ;

arithmeticParantheses	: LPARAN arithmeticExpression RPARAN	{ $$ = $2; }
                        | NUMBER                        	{ $$ = new NumberExpression(atol($1->c_str())); delete $1; }
                        | IDENTIFIER                            { $$ = new IdentifierExpression(*$1, @1.first_column); delete $1; }
                        ;

parExpression		: LPARAN expression RPARAN  { $$ = new VerifyTAPN::AST::ParExpression($2); };
notExpression		: NOT parExpression         { $$ = new VerifyTAPN::AST::NotExpression($2); };
orExpression		: expression OR expression  { $$ = new VerifyTAPN::AST::OrExpression($1, $3); };
andExpression		: expression AND expression { $$ = new VerifyTAPN::AST::AndExpression($1, $3); };
boolExpression		: BOOL_TRUE                 { $$ = new VerifyTAPN::AST::BoolExpression(true); } 
			| BOOL_FALSE                { $$ = new VerifyTAPN::AST::BoolExpression(false); };

atomicProposition	: arithmeticExpression compareOp arithmeticExpression { $$ = new VerifyTAPN::AST::AtomicProposition($1, $2, $3); } 
                        | DEADLOCK { $$ = new VerifyTAPN::AST::DeadlockExpression(); } ;

compareOp		: LESS | LESSEQUAL | EQUAL | GREATEREQUAL | GREATER; 
     
%%

void 
VerifyTAPN::Parser::error (const VerifyTAPN::Parser::location_type& l,
                        const std::string& m)
{
	driver.error (l, m);
	exit(1);
}
