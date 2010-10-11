#ifndef TAPNQUERYPARSER_HPP_
#define TAPNQUERYPARSER_HPP_

#include "QueryGrammar.hpp"
#include <string>

namespace VerifyTAPN
{
	namespace Query
	{
		class TAPNQueryParser
		{
		public:
			TAPNQueryParser() : grammar() {};
			virtual ~TAPNQueryParser() { };

		public: // factory methods
			void Parse(const std::string& file) const;
		public: // factory methods
			QueryGrammar<std::string::const_iterator> grammar;
		};
	}
}

#endif /* TAPNQUERYPARSER_HPP_ */
