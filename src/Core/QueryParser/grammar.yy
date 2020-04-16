%skeleton "lalr1.cc"                          /*  -*- C++ -*- */
%require "2.4"
%defines
%define parser_class_name "Parser"
%define namespace "VerifyTAPN"

%code requires {
#include <string>
#include "Core/QueryParser/AST.hpp"
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
        VerifyTAPN::AST::ArithmeticExpression*           arexpr;
	VerifyTAPN::AST::Query*                         query;
};

%code{
	#include "Core/QueryParser/TAPNQueryParser.hpp"
	#include "lexer_def.h"
}

%token <string> IDENTIFIER LESS LESSEQUAL EQUAL GREATEREQUAL GREATER
%token <number> NUMBER    
%token END      0
%token EF AG AF EG CONTROL
%token LPARAN RPARAN COLON
%token OR AND NOT
%token BOOL_TRUE BOOL_FALSE
%token DEADLOCK
%token PLUS MINUS MULTIPLY
%type <expr> expression notExpression parExpression orExpression andExpression boolExpression atomicProposition
%type <arexpr> arithmeticExpression multiplyExpression arithmeticParantheses
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
                        | CONTROL COLON AF expression {$$ = new VerifyTAPN::AST::Query(VerifyTAPN::AST::CF, $4); driver.setAST($$);}
                        | CONTROL COLON AG expression {$$ = new VerifyTAPN::AST::Query(VerifyTAPN::AST::CG, $4); driver.setAST($$);}
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

arithmeticExpression	: arithmeticExpression PLUS multiplyExpression	{ $$ = new VerifyTAPN::AST::PlusExpression($1, $3); }
                        | arithmeticExpression MINUS multiplyExpression	{ $$ = new VerifyTAPN::AST::SubtractExpression($1, $3); }
                        | MINUS multiplyExpression                    { $$ = new VerifyTAPN::AST::MinusExpression($2); }
                        | multiplyExpression                            { $$ = $1; }
                        ;

multiplyExpression	: multiplyExpression MULTIPLY arithmeticParantheses	{ $$ = new VerifyTAPN::AST::MultiplyExpression($1, $3); }
                        | arithmeticParantheses                                 { $$ = $1; }
                        ;

arithmeticParantheses	: LPARAN arithmeticExpression RPARAN	{ $$ = $2; }
                        | NUMBER                        	{ $$ = new VerifyTAPN::AST::NumberExpression($1);}
                        | IDENTIFIER                            { 
                                                                    int placeIndex = driver.getTAPN().getPlaceIndex(*$1);
                                                                    delete $1;
                                                                    if(placeIndex == -1) error(@1, "unknown place"); 
                                                                    $$ = new VerifyTAPN::AST::IdentifierExpression(placeIndex); 
                                                                 }
                        ;

parExpression		: LPARAN expression RPARAN  { $$ = $2; };
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
