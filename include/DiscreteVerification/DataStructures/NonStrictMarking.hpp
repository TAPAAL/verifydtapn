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

using namespace std;

namespace VerifyTAPN {
    namespace DiscreteVerification {

        class NonStrictMarking : public NonStrictMarkingBase {
        public:
            NonStrictMarking() : NonStrictMarkingBase(), meta(NULL) {}

            NonStrictMarking(const TAPN::TimedArcPetriNet &tapn, const std::vector<int> &v) : NonStrictMarkingBase(tapn,
                                                                                                                   v),
                                                                                              meta(NULL) {}

            NonStrictMarking(const NonStrictMarkingBase &nsm) : NonStrictMarkingBase(nsm), meta(NULL) {

            }

            NonStrictMarking(const NonStrictMarking &nsm) : NonStrictMarkingBase(nsm), meta(NULL) {

            }

            inline int calculateTotalDelay() {
                int totalDelay = 0;
                NonStrictMarking *parent = (NonStrictMarking *) this->getParent();
                if (parent && parent->meta) {
                    totalDelay = parent->meta->totalDelay;
                    if (this->getGeneratedBy() == NULL) ++totalDelay;
                }
                return totalDelay;
            }

        public:
            MetaData *meta;
        };
    } /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */

#endif /* NONSTRICTMARKING_HPP_ */
