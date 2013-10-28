/*
 * NonStrictSearch.cpp
 *
 *  Created on: 26/04/2012
 *      Author: MathiasGS
 */

#include "Workflow.hpp"

namespace VerifyTAPN {
    namespace DiscreteVerification {

        bool Workflow::isDelayPossible(NonStrictMarking& marking) {
            const PlaceList& places = marking.getPlaceList();
            if (places.size() == 0) return true; //Delay always possible in empty markings

            PlaceList::const_iterator markedPlace_iter = places.begin();
            for (TAPN::TimedPlace::Vector::const_iterator place_iter = tapn.getPlaces().begin(); place_iter != tapn.getPlaces().end(); place_iter++) {
                int inv = (*place_iter)->getInvariant().getBound();
                if (**place_iter == *(markedPlace_iter->place)) {
                    if (markedPlace_iter->maxTokenAge() > inv - 1) {
                        return false;
                    }

                    markedPlace_iter++;

                    if (markedPlace_iter == places.end()) return true;
                }
            }
            assert(false); // This happens if there are markings on places not in the TAPN
            return false;
        }
    } /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
