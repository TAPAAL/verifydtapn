#include "TransportArc.hpp"
#include "TimedTransition.hpp"
#include "TimedPlace.hpp"

namespace VerifyTAPN
{
	namespace TAPN
	{
		void TransportArc::Print(std::ostream& out) const
		{
			out << "From " << source->GetName() << " to " << transition->GetName() << " to " << destination->GetName();
			out << " with interval " << interval;
		}
	}
}
