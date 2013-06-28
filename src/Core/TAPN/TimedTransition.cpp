#include "TimedTransition.hpp"

namespace VerifyTAPN {
	namespace TAPN {
		void TimedTransition::print(std::ostream& out) const
		{
			out << getName();
                        if(this->urgent)
                            out << " urgent ";
                        out << "(" << index << ")";
		}

		void TimedTransition::addToPreset(const boost::shared_ptr<TimedInputArc>& arc)
		{
			if(arc)
			{
                                if(this->urgent){   // all urgency in discrete time must have untimed 
                                                    //inputarcs to not break semantics
                                    if(!arc.get()->getInterval().isZeroInfinity()){
                                        std::cout << "Urgent transitions must have untimed input arcs" << std::endl;
                                        exit(1);
                                    }
                                }
                                preset.push_back(arc);
			}
		}

		void TimedTransition::addTransportArcGoingThrough(const boost::shared_ptr<TransportArc>& arc)
		{
			if(arc)
			{
                            if(this->urgent){   // all urgency in discrete time must have untimed 
                                                //inputarcs to not break semantics
                                if(!arc.get()->getInterval().isZeroInfinity()){
                                    std::cout << "Urgent transitions must have untimed transportarcs" << std::endl;
                                    exit(1);
                                } else if(!arc.get()->getDestination().isUntimed()){
                                    // urgency breaks if we have invariant at destination
                                    std::cout << "Transportarcs going through an urgent transition cannot have invariants at destination-places." << std::endl;
                                    exit(1);                                    
                                }
                            }
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




