#include "OutputArc.hpp"
#include "TimedPlace.hpp"
#include "TimedTransition.hpp"

namespace VerifyTAPN {
	namespace TAPN {
		void OutputArc::print(std::ostream& out) const
		{
			out << "From " << transition->GetName() << " to " << place->getName() << " weight: " << weight;
		}

		TimedPlace& OutputArc::getOutputPlace()
		{
			return *place;
		}

		TimedTransition& OutputArc::getInputTransition()
		{
			return *transition;
		}
	}
}
