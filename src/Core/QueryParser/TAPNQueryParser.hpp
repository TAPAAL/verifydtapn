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
		void SetAST(VerifyTAPN::AST::Query* ast);
		AST::Query* GetAST();

		const VerifyTAPN::TAPN::TimedArcPetriNet& tapn() { return net; };

	public: // error handling
		void error(const location& l, const std::string& m);
		void error(const std::string& m);

	public:
		std::string file;
		VerifyTAPN::AST::Query* ast;
		const VerifyTAPN::TAPN::TimedArcPetriNet& net;
	};
}

#endif /* DRIVER_HPP_ */
