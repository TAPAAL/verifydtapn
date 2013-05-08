#include "TimedInputArc.hpp"
#include "TimedTransition.hpp"
#include "TimedPlace.hpp"

namespace VerifyTAPN {
	namespace TAPN {
		void TimedInputArc::print(std::ostream& out) const
		{
			out << "From " << place->getName() << " to " << transition->GetName() << " weight: " << weight;
			out << " with interval " << interval;
		}
	}
}
