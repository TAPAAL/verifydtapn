#include "TimeInvariant.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/lexical_cast.hpp"

namespace VerifyTAPN {
	namespace TAPN {
		const TimeInvariant TimeInvariant::LS_INF;

		TimeInvariant TimeInvariant::CreateFor(const std::string& invariant)
		{
			bool strict = !boost::algorithm::icontains(invariant, "<=");
			int bound = std::numeric_limits<int>().max();

			int pos = strict ? 1 : 2;
			std::string number = invariant.substr(pos);
			boost::algorithm::trim(number);

			if(!boost::algorithm::icontains(invariant, "inf"))
			{
				bound = boost::lexical_cast<int>(number);
			}

			return TimeInvariant(strict, bound);
		}

		void TimeInvariant::Print(std::ostream& out) const
		{
			std::string comparison = strictComparison ? "<" : "<=";
			std::string strBound = bound == std::numeric_limits<int>().max() ? "inf" : boost::lexical_cast<std::string>(bound);

			out << comparison << " " << strBound;
		}
	}
}
