#include "TimedTransition.hpp"
#include "../Core/SymMarking.hpp"
#include "../Core/DiscretePart.hpp"
#include "TimedArcPetriNet.hpp"

namespace VerifyTAPN {
    namespace TAPN {

        void TimedTransition::Print(std::ostream& out) const {
            out << GetName();
        }

        void TimedTransition::AddToPreset(const std::shared_ptr<TimedInputArc>& arc) {
            if (arc) {
                preset.push_back(arc);
            }
        }

        void TimedTransition::AddToPostset(const std::shared_ptr<OutputArc>& arc) {
            if (arc) {
                postset.push_back(arc);
            }
        }
    }
}




