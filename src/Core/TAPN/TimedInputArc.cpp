#include "Core/TAPN/TimedInputArc.hpp"
#include "Core/TAPN/TimedTransition.hpp"
#include "Core/TAPN/TimedPlace.hpp"

namespace VerifyTAPN::TAPN {
        void TimedInputArc::print(std::ostream &out) const {
            out << "From " << place.getName() << " to " << transition.getName() << " weight: " << weight;
            out << " with interval " << interval;
        }
    }
