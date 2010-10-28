#include "TimedInputArc.hpp"
#include "TimedTransition.hpp"
#include "TimedPlace.hpp"

namespace VerifyTAPN {
	namespace TAPN {
		void TimedInputArc::Print(std::ostream& out) const
		{
			out << "From " << place->GetName() << " to " << transition->GetName();
			out << " with interval " << interval;
		}
	}
}
