/*
 * WaitingList.cpp
 *
 *  Created on: 16/05/2015
 *      Author: Peter G. Jensen
 */

#include "WaitingList.hpp"
namespace VerifyTAPN {
namespace DiscreteVerification {
    template <>
    int MinFirstWaitingList<NonStrictMarking*>::calculateWeight(NonStrictMarking* marking)
    {
            return marking->meta->totalDelay;
    }

    template <>
    int MinFirstWaitingList<ptriepointer<MetaData*> >::calculateWeight(ptriepointer<MetaData*> payload)
    {
        return payload.get_meta()->totalDelay;
    }
}
}