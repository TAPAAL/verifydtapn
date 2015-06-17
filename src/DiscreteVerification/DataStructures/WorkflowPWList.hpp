/*
 * PWList.hpp
 *
 *  Created on: 01/03/2012
 *      Author: MathiasGS
 */

#ifndef WORKFLOWPWLIST_HPP_
#define WORKFLOWPWLIST_HPP_

#include "WaitingList.hpp"
#include <iostream>
#include "google/sparse_hash_map"
#include  "NonStrictMarking.hpp"
#include "NonStrictMarking.hpp"
#include "WaitingList.hpp"
#include "PWList.hpp"
#include "CoveredMarkingVisitor.h"

namespace VerifyTAPN {
namespace DiscreteVerification {

    class WorkflowPWList : public PWList {

    public:
        WorkflowPWList(WaitingList<NonStrictMarking*>* w_l);
    	NonStrictMarking* getCoveredMarking(NonStrictMarking* marking, bool useLinearSweep);
        NonStrictMarking* getUnpassed();
    	virtual bool add(NonStrictMarking* marking);
        NonStrictMarking* addToPassed(NonStrictMarking* marking);
        NonStrictMarking* lookup(NonStrictMarking* marking);

    };
    
    class WorkflowPWListHybrid : public PWListHybrid {
    private:
        CoveredMarkingVisitor visitor;
    public:
        WorkflowPWListHybrid(TAPN::TimedArcPetriNet& tapn, 
                                WaitingList<ptriepointer_t<MetaData*> >* w_l, 
                                int knumber, 
                                int nplaces, 
                                int mage, 
                                bool makeTrace);
    	NonStrictMarking* getCoveredMarking(NonStrictMarking* marking, bool useLinearSweep);
        NonStrictMarking* getUnpassed();
    	virtual bool add(NonStrictMarking* marking);
        NonStrictMarking* addToPassed(NonStrictMarking* marking);
    };

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* PWLIST_HPP_ */
