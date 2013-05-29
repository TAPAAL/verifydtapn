#ifndef DRIVER_HPP_
#define DRIVER_HPP_

#include "Generated/parser.hpp"
#include <string>
#include "../TAPN/TimedArcPetriNet.hpp"

# define YY_DECL                                        \
  VerifyTAPN::Parser::token_type                         \
  yylex (VerifyTAPN::Parser::semantic_type* yylval,      \
         VerifyTAPN::Parser::location_type* yylloc,      \
         VerifyTAPN::TAPNQueryParser& driver)

YY_DECL;

namespace VerifyTAPN
{

	namespace AST{
		class Query;
	}

	class TAPNQueryParser {
	public:
		TAPNQueryParser(const VerifyTAPN::TAPN::TimedArcPetriNet& tapn) : ast(NULL), net(tapn) { };
		virtual ~TAPNQueryParser() { };

	private:
		void scan_begin();
		void scan_end();

	public:
		int parse(const std::string& file);
		void setAST(VerifyTAPN::AST::Query* ast);
		AST::Query* getAST();

		const VerifyTAPN::TAPN::TimedArcPetriNet& getTAPN() { return net; };

	public: // error handling
		void error(const location& l, const std::string& m);
		void error(const std::string& m);

                // must be public for the generated lexer and parser to work!
		std::string file;
        private:
		VerifyTAPN::AST::Query* ast;
		const VerifyTAPN::TAPN::TimedArcPetriNet& net;
	};
}

#endif /* DRIVER_HPP_ */
