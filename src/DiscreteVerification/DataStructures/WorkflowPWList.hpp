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

namespace VerifyTAPN {
namespace DiscreteVerification {

    class WorkflowPWListBasic : virtual public PWListBase
    {
    public:
    	virtual NonStrictMarking* getCoveredMarking(NonStrictMarking* marking, bool useLinearSweep) = 0;
        virtual NonStrictMarking* getUnpassed() = 0;
    	virtual bool add(NonStrictMarking* marking) = 0;
        virtual NonStrictMarking* addToPassed(NonStrictMarking* marking, bool strong) = 0;
        virtual void addLastToWaiting() = 0;
        virtual void setParent(NonStrictMarking*, NonStrictMarking*) = 0;
    };
    
    class WorkflowPWList : public WorkflowPWListBasic, public PWList {
    private:
        NonStrictMarking* last;
    public:
        WorkflowPWList(WaitingList<NonStrictMarking*>* w_l);
    	virtual NonStrictMarking* getCoveredMarking(NonStrictMarking* marking, bool useLinearSweep);
        virtual NonStrictMarking* getUnpassed();
    	virtual bool add(NonStrictMarking* marking);
        virtual NonStrictMarking* addToPassed(NonStrictMarking* marking, bool strong);
        NonStrictMarking* lookup(NonStrictMarking* marking);
        
        virtual void addLastToWaiting(){
		waiting_list->add(last, last);
	}
        
        virtual void setParent(NonStrictMarking* marking, NonStrictMarking* parent)
        {
            marking->setParent(parent);
        }

    };

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* PWLIST_HPP_ */
