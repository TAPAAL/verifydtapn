#include "TimedTransition.hpp"
#include "../SymbolicMarking/SymMarking.hpp"
#include "../SymbolicMarking/DiscretePart.hpp"
#include "TimedArcPetriNet.hpp"

namespace VerifyTAPN {
	namespace TAPN {
		void TimedTransition::Print(std::ostream& out) const
		{
			out << GetName();
		}

		void TimedTransition::AddToPreset(const boost::shared_ptr<TimedInputArc>& arc)
		{
			if(arc)
			{
				preset.push_back(arc);
			}
		}

		void TimedTransition::AddToPostset(const boost::shared_ptr<OutputArc>& arc)
		{
			if(arc)
			{
				postset.push_back(arc);
			}
		}
	}
}




