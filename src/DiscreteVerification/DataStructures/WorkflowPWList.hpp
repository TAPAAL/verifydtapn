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

    class WorkflowPWList : public PWList {
    public:
    	WorkflowPWList(WaitingList<NonStrictMarking>* w_l) : PWList(w_l, false) {};
    	bool add(NonStrictMarking* marking){
    		discoveredMarkings++;
    		NonStrictMarkingList& m = markings_storage[marking->getHashKey()];
    		for(NonStrictMarkingList::const_iterator iter = m.begin();
    				iter != m.end();
    				iter++){
    			if((*iter)->equals(*marking)){
 	                    return false;
    			}
    		}
    	        stored++;
    		m.push_back(marking);
    		waiting_list->add(marking, marking);
    		return true;
    	}

    	NonStrictMarking* addToPassed(NonStrictMarking* marking){
    			NonStrictMarkingList& m = markings_storage[marking->getHashKey()];
    			for(NonStrictMarkingList::const_iterator iter = m.begin();
    						iter != m.end();
    						iter++){
    					if((*iter)->equals(*marking)){
    						return *iter;
    					}
    			}
    			stored++;
    			m.push_back(marking);
    			return marking;
    		}

    };

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* PWLIST_HPP_ */
