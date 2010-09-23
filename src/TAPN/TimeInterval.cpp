#include "TimeInterval.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/lexical_cast.hpp"
#include <vector>

namespace VerifyTAPN {
	namespace TAPN {
		using namespace boost::algorithm;
		TimeInterval TimeInterval::CreateFor(const std::string& interval)
		{
			bool leftStrict = boost::algorithm::icontains(interval, "(");
			bool rightStrict = boost::algorithm::icontains(interval,")");

			std::vector<std::string> splitVector;
			split(splitVector, interval, is_any_of(","));

			std::string strLowerBound = splitVector[0].substr(1);
			trim(strLowerBound);
			std::string strUpperBound = splitVector[1].substr(0,splitVector[1].size()-1);
			trim(strUpperBound);

			int lowerBound = boost::lexical_cast<int>(strLowerBound);
			int upperBound = std::numeric_limits<int>().max();

			if(!iequals(strUpperBound, "inf"))
			{
				upperBound = boost::lexical_cast<int>(strUpperBound);
			}

			return TimeInterval(leftStrict, lowerBound, upperBound, rightStrict);
		}

		void TimeInterval::Print(std::ostream& out) const
		{
			std::string leftParenthesis = leftStrict ? "(" : "[";
			std::string rightParenthesis = rightStrict ? ")" : "]";

			std::string strLowerBound = boost::lexical_cast<std::string>(lowerBound);
			std::string strUpperBound = upperBound == std::numeric_limits<int>().max() ? "inf" : boost::lexical_cast<std::string>(upperBound);

			out << leftParenthesis << strLowerBound << "," << strUpperBound << rightParenthesis;
		}
	}
}
