#include "TimedTransition.hpp"

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

		void TimedTransition::AddTransportArcGoingThrough(const boost::shared_ptr<TransportArc>& arc)
		{
			if(arc)
			{
				transportArcs.push_back(arc);
			}
		}

		void TimedTransition::AddIncomingInhibitorArc(const boost::shared_ptr<InhibitorArc>& arc)
		{
			if(arc)
			{
				inhibitorArcs.push_back(arc);
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




