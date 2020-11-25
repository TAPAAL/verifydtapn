#include "Core/TAPN/InhibitorArc.hpp"
#include "Core/TAPN/TimedTransition.hpp"
#include "Core/TAPN/TimedPlace.hpp"

namespace VerifyTAPN { namespace TAPN {
    void InhibitorArc::print(std::ostream &out) const {
        out << "From " << place.getName() << " to " << transition.getName() << " weight: " << weight;
    }
} }
