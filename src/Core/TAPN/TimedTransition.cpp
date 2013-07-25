#include "TimedTransition.hpp"
#include "TimedPlace.hpp"

namespace VerifyTAPN {
    namespace TAPN {

        void TimedTransition::print(std::ostream& out) const {
            out << getName();
            if (this->urgent)
                out << " urgent ";
            out << "(" << index << ")";
        }

        void TimedTransition::addToPreset(TimedInputArc& arc) {

            if (this->urgent) { // all urgency in discrete time must have untimed 
                //inputarcs to not break semantics
                if (!arc.getInterval().isZeroInfinity()) {
                    std::cout << "Urgent transitions must have untimed input arcs" << std::endl;
                    exit(1);
                }
            }
            preset.push_back(&arc);
        }

        void TimedTransition::addTransportArcGoingThrough(TransportArc& arc) {
            if (this->urgent) { // all urgency in discrete time must have untimed 
                //inputarcs to not break semantics
                if (!arc.getInterval().isZeroInfinity()) {
                    std::cout << "Urgent transitions must have untimed transportarcs" << std::endl;
                    exit(1);
                } else if (arc.getDestination().getInvariant() != TimeInvariant::LS_INF) {
                    // urgency breaks if we have invariant at destination
                    std::cout << "Transportarcs going through an urgent transition cannot have invariants at destination-places." << std::endl;
                    exit(1);
                }
            }
            transportArcs.push_back(&arc);
        }

        void TimedTransition::addIncomingInhibitorArc(InhibitorArc& arc) {
            inhibitorArcs.push_back(&arc);
        }

        void TimedTransition::addToPostset(OutputArc& arc) {
            postset.push_back(&arc);
        }
    }
}




