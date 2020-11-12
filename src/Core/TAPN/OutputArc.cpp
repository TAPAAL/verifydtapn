#include "Core/TAPN/OutputArc.hpp"
#include "Core/TAPN/TimedPlace.hpp"
#include "Core/TAPN/TimedTransition.hpp"

namespace VerifyTAPN { namespace TAPN {
    void OutputArc::print(std::ostream &out) const {
        out << "From " << transition.getName() << " to " << place.getName() << " weight: " << weight;
    }

    TimedPlace &OutputArc::getOutputPlace() const {
        return place;
    }

    TimedTransition &OutputArc::getInputTransition() const {
        return transition;
    }
} }
