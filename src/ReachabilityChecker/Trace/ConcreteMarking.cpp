#include "ConcreteMarking.hpp"
#include <ostream>
#include "../../Core/TAPN/TimedPlace.hpp"

namespace VerifyTAPN
{
	std::ostream& Token::Print(std::ostream& out) const
	{
		if(place != TAPN::TimedPlace::BOTTOM_NAME) out << "(" << place << "," << age << ")";
		return out;
	}

	std::ostream& ConcreteMarking::Print(std::ostream& out) const
	{
		out << "Marking: ";
		for(std::deque<Token>::const_iterator iter = tokens.begin(); iter != tokens.end(); ++iter)
		{
			out << *iter << " ";
		}
		out << std::endl;
		return out;
	}
}
