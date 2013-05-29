#include "TimedTransition.hpp"

namespace VerifyTAPN {
	namespace TAPN {
		void TimedTransition::print(std::ostream& out) const
		{
			out << getName() << "(" << index << ")";
		}

		void TimedTransition::addToPreset(const boost::shared_ptr<TimedInputArc>& arc)
		{
			if(arc)
			{
				preset.push_back(arc);
			}
		}

		void TimedTransition::addTransportArcGoingThrough(const boost::shared_ptr<TransportArc>& arc)
		{
			if(arc)
			{
				transportArcs.push_back(arc);
			}
		}

		void TimedTransition::addIncomingInhibitorArc(const boost::shared_ptr<InhibitorArc>& arc)
		{
			if(arc)
			{
				inhibitorArcs.push_back(arc);
			}
		}

		void TimedTransition::addToPostset(const boost::shared_ptr<OutputArc>& arc)
		{
			if(arc)
			{
				postset.push_back(arc);
			}
		}
	}
}




