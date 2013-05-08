#include "TransportArc.hpp"
#include "TimedTransition.hpp"
#include "TimedPlace.hpp"

namespace VerifyTAPN
{
	namespace TAPN
	{
		void TransportArc::Print(std::ostream& out) const
		{
			out << "From " << source->getName() << " to " << transition->GetName() << " to " << destination->getName() << " weight: " << weight;
			out << " with interval " << interval;
		}
	}
}
