#include "Core/TAPN/TransportArc.hpp"
#include "Core/TAPN/TimedTransition.hpp"
#include "Core/TAPN/TimedPlace.hpp"

namespace VerifyTAPN { namespace TAPN {
    void TransportArc::print(std::ostream &out) const {
        out << "From " << source.getName() << " to " << transition.getName() << " to " << destination.getName()
            << " weight: " << weight;
        out << " with interval " << interval;
    }
} }
