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
	VerifyTAPN::AST::Expression*	expr;
	VerifyTAPN::AST::Query*		 	query;
};

%code{
	#include "../TAPNQueryParser.hpp"
}

%token <string> IDENTIFIER LESS LESSEQUAL EQUAL GREATEREQUAL GREATER
%token <number> NUMBER    
%token END      0
%token EF AG
%token LPARAN RPARAN
%token OR AND
%token BOOL_TRUE BOOL_FALSE
%type  <expr> expression parExpression orExpression andExpression atomicProposition
%type <query> query
%type <string> compareOp

%destructor { delete $$; } IDENTIFIER LESS LESSEQUAL EQUAL GREATEREQUAL GREATER compareOp
%destructor { delete $$; } expression parExpression orExpression andExpression atomicProposition
%destructor { delete $$; } query

%%
%start query;
query				: EF expression { $$ = new VerifyTAPN::AST::Query(VerifyTAPN::AST::EF, $2); driver.SetAST($$); }
					| AG expression { $$ = new VerifyTAPN::AST::Query(VerifyTAPN::AST::AG, $2); driver.SetAST($$); }
;

expression			: parExpression { $$ = $1; }
					| orExpression { $$ = $1; }
					| andExpression { $$ = $1; }
					| atomicProposition { $$ = $1; }
					| BOOL_TRUE { $$ = new VerifyTAPN::AST::BoolExpression(true); } 
					| BOOL_FALSE { $$ = new VerifyTAPN::AST::BoolExpression(false); }
;

%left OR;
%left AND;

parExpression		: LPARAN expression RPARAN { $$ = new VerifyTAPN::AST::ParExpression($2); };
orExpression		: expression OR expression { $$ = new VerifyTAPN::AST::OrExpression($1, $3); };
andExpression		: expression AND expression { $$ = new VerifyTAPN::AST::AndExpression($1, $3); };
atomicProposition	: IDENTIFIER compareOp NUMBER 
	{ 
		int placeIndex = driver.tapn().GetPlaceIndex(*$1);
		if(placeIndex == -1) error(@1, "unknown place"); 
		$$ = new VerifyTAPN::AST::AtomicProposition(placeIndex, $2, $3); 
	};
compareOp			: LESS | LESSEQUAL | EQUAL | GREATEREQUAL | GREATER; 
     
%%

void 
VerifyTAPN::Parser::error (const VerifyTAPN::Parser::location_type& l,
                        const std::string& m)
{
	driver.error (l, m);
	exit(1);
}
