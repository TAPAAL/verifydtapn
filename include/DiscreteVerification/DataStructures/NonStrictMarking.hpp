/*
 * NonStrictMarking.hpp
 *
 *  Created on: 29/02/2012
 *      Author: MathiasGS
 */

#ifndef NONSTRICTMARKING_HPP_
#define NONSTRICTMARKING_HPP_

#include "NonStrictMarkingBase.hpp"
#include "Core/TAPN/TAPN.hpp"
#include "MetaData.h"

#include <vector>
#include <iostream>

namespace VerifyTAPN { namespace DiscreteVerification {

    class NonStrictMarking : public NonStrictMarkingBase {
    public:
        NonStrictMarking() : NonStrictMarkingBase(), meta(nullptr) {}

        NonStrictMarking(const TAPN::TimedArcPetriNet &tapn, const std::vector<int> &v) : NonStrictMarkingBase(tapn,
                                                                                                               v),
                                                                                          meta(nullptr) {}

        explicit NonStrictMarking(const NonStrictMarkingBase &nsm) : NonStrictMarkingBase(nsm), meta(nullptr) {

        }

        NonStrictMarking(const NonStrictMarking &nsm) : NonStrictMarkingBase(nsm), meta(nullptr) {

        }

        inline int calculateTotalDelay() {
            int totalDelay = 0;
            auto *parent = (NonStrictMarking *) this->getParent();
            if (parent && parent->meta) {
                totalDelay = parent->meta->totalDelay;
                if (this->getGeneratedBy() == nullptr) ++totalDelay;
            }
            return totalDelay;
        }

    public:
        MetaData *meta;
    };
} } /* namespace VerifyTAPN */

#endif /* NONSTRICTMARKING_HPP_ */
