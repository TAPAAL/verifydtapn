#include "Core/TAPN/TimedTransition.hpp"
#include "Core/TAPN/TimedPlace.hpp"

#include <cstdlib>

namespace VerifyTAPN {
namespace TAPN {

    void TimedTransition::print(std::ostream &out) const {
        out << getName();
        if (this->urgent)
            out << " urgent ";
        out << "(" << index << ")";
    }

    void TimedTransition::addToPreset(TimedInputArc &arc) {

        if (this->urgent) { // all urgency in discrete time must have untimed
            //inputarcs to not break semantics
            if (!arc.getInterval().isZeroInfinity()) {
                std::cout << "Urgent transitions must have untimed input arcs" << std::endl;
               std::exit(1);
            }
        }

        for (auto it = preset.begin(); it != preset.end(); ++it) {
            if ((*it)->getInputPlace().getIndex() > arc.getInputPlace().getIndex()) {
                preset.insert(it, &arc);
                return;
            }
        }
        preset.push_back(&arc);
    }

    void TimedTransition::addTransportArcGoingThrough(TransportArc &arc) {
        if (this->urgent) { // all urgency in discrete time must have untimed
            //inputarcs to not break semantics
            if (!arc.getInterval().isZeroInfinity()) {
                std::cout << "Urgent transitions must have untimed transportarcs" << std::endl;
               std::exit(1);
            } else if (arc.getDestination().getInvariant() != TimeInvariant::LS_INF) {
                // urgency breaks if we have invariant at destination
                std::cout
                        << "Transportarcs going through an urgent transition cannot have invariants at destination-places."
                        << std::endl;
               std::exit(1);
            }
        }


        for (auto it = transportArcs.begin(); it != transportArcs.end(); ++it) {
            if ((*it)->getSource().getIndex() > arc.getSource().getIndex()) {
                transportArcs.insert(it, &arc);
                return;
            }
        }
        transportArcs.push_back(&arc);
    }

    void TimedTransition::addIncomingInhibitorArc(InhibitorArc &arc) {
        for (auto it = inhibitorArcs.begin(); it != inhibitorArcs.end(); ++it) {
            if ((*it)->getInputPlace().getIndex() > arc.getInputPlace().getIndex()) {
                inhibitorArcs.insert(it, &arc);
                return;
            }
        }
        inhibitorArcs.push_back(&arc);
    }

    void TimedTransition::addToPostset(OutputArc &arc) {
        for (auto it = postset.begin(); it != postset.end(); ++it) {
            if ((*it)->getOutputPlace().getIndex() > arc.getOutputPlace().getIndex()) {
                postset.insert(it, &arc);
                return;
            }
        }
        postset.push_back(&arc);
    }
}
}
