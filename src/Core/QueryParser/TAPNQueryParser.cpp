#include "TAPNQueryParser.hpp"
#include "../util.hpp"
#include "boost/spirit/include/qi_parse.hpp"
#include "boost/spirit/include/qi.hpp"

namespace VerifyTAPN
{
	namespace Query
	{
		void TAPNQueryParser::Parse(const std::string& file) const
		{
			std::string contents = ReadFile(file);
			std::string::const_iterator begin = contents.begin();
			std::string::const_iterator end = contents.end();

			bool succ = phrase_parse(begin, end, grammar, boost::spirit::ascii::space);
		}
	}
}
