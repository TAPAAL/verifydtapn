#include "InhibitorArc.hpp"
#include "TimedTransition.hpp"
#include "TimedPlace.hpp"

namespace VerifyTAPN {
	namespace TAPN {
		void InhibitorArc::print(std::ostream& out) const
		{
			out << "From " << place->getName() << " to " << transition->getName() << " weight: " << weight;
		}
	}
}
