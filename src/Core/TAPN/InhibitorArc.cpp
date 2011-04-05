#include "InhibitorArc.hpp"
#include "TimedTransition.hpp"
#include "TimedPlace.hpp"

namespace VerifyTAPN {
	namespace TAPN {
		void InhibitorArc::Print(std::ostream& out) const
		{
			out << "From " << place->GetName() << " to " << transition->GetName();
		}
	}
}
