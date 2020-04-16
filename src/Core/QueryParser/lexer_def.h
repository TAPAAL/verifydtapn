//
// Created by Peter G. Jensen on 16/04/2020.
//

#ifndef VERIFYDTAPN_LEXER_DEF_H
#define VERIFYDTAPN_LEXER_DEF_H

#include "Parser.hpp"

# define YY_DECL                                        \
  VerifyTAPN::Parser::token_type                         \
  yylex (VerifyTAPN::Parser::semantic_type* yylval,      \
         VerifyTAPN::Parser::location_type* yylloc,      \
         VerifyTAPN::TAPNQueryParser& driver)

    YY_DECL;
#endif //VERIFYDTAPN_LEXER_DEF_H
