#ifndef CLOCKVALUE_H
#define CLOCKVALUE_H

#include <cstdint>

#include "Core/TAPN/TimedArcPetriNet.hpp"

namespace VerifyTAPN::DiscreteVerification::Util {

    typedef uint64_t clockValue;

    double clockToDouble(clockValue value, const uint32_t precision);

    clockValue toClock(double value, const uint32_t precision);

    clockValue toClock(int value, const uint32_t precision);

}

#endif
