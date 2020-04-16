#include "Core/QueryParser/TAPNQueryParser.hpp"
#include "Parser.hpp"
#include <fstream>

namespace VerifyTAPN
{

	int TAPNQueryParser::parse(const std::string& filepath)
	{
		this->file = filepath;
		scan_begin();
		Parser parser(*this);
		int result = parser.parse();
		scan_end();
		return result;
	}

	void TAPNQueryParser::setAST(AST::Query* query){
		ast = query->clone();
	}

	AST::Query* TAPNQueryParser::getAST(){
		return ast;
	}

	void TAPNQueryParser::error(const location& l, const std::string& m)
	{
		std::cerr << l << ": " << m << std::endl;
	}

	void TAPNQueryParser::error(const std::string& m)
	{
		std::cerr << m << std::endl;
	}
}
