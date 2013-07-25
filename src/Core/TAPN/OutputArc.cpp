#include "OutputArc.hpp"
#include "TimedPlace.hpp"
#include "TimedTransition.hpp"

namespace VerifyTAPN {
	namespace TAPN {
		void OutputArc::print(std::ostream& out) const
		{
			out << "From " << transition.getName() << " to " << place.getName() << " weight: " << weight;
		}

		TimedPlace& OutputArc::getOutputPlace() const
		{
			return place;
		}

		TimedTransition& OutputArc::getInputTransition() const
		{
			return transition;
		}
	}
}
