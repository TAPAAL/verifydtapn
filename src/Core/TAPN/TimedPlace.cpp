#include "Core/TAPN/TimedPlace.hpp"

namespace VerifyTAPN::TAPN {

    const std::string TimedPlace::BOTTOM_NAME = "*BOTTOM*";

    const std::string &TimedPlace::getName() const {
        return name;
    }

    const std::string &TimedPlace::getId() const {
        return id;
    }

    void TimedPlace::print(std::ostream &out) const {
        out << "(" << name << " (index: " << index << "), " << timeInvariant << ", Max Constant: " << maxConstant
            << ", Infinity Place: " << (untimed ? "true" : "false") << ", Type: "
            << (type == Std ? "Std" : (type == Inv ? "Inv" : "Dead")) << ")";
    }

    void TimedPlace::divideInvariantBy(int divider) {
        if (timeInvariant.getBound() != 0 && timeInvariant.getBound() != std::numeric_limits<int>::max()) {
            timeInvariant = TAPN::TimeInvariant(
                    timeInvariant.isBoundStrict(),
                    timeInvariant.getBound() / divider);
        }
    }
}
