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
    	NonStrictMarking* getCoveredMarking(NonStrictMarking* marking, bool useLinearSweep){
			if(useLinearSweep){
				for(HashMap::const_iterator iter = markings_storage.begin(); iter != markings_storage.end(); ++iter){
					for(NonStrictMarkingList::const_iterator m_iter = iter->second.begin(); m_iter != iter->second.end(); m_iter++){
						if((*m_iter)->size() >= marking->size()){
							continue;
						}

						// Test if m_iter is covered by marking
						PlaceList::const_iterator marking_place_iter = marking->getPlaceList().begin();

						bool tokensCovered = true;
						for(PlaceList::const_iterator m_place_iter = (*m_iter)->getPlaceList().begin(); m_place_iter != (*m_iter)->getPlaceList().end(); ++m_place_iter){
							while(marking_place_iter != marking->getPlaceList().end() && marking_place_iter->place != m_place_iter->place){
								++marking_place_iter;
							}

							if(marking_place_iter == marking->getPlaceList().end()){
								tokensCovered = false;
								break;	// Place not covered in marking
							}

							TokenList::const_iterator marking_token_iter = marking_place_iter->tokens.begin();
							for(TokenList::const_iterator m_token_iter = m_place_iter->tokens.begin(); m_token_iter != m_place_iter->tokens.end(); ++m_token_iter){
								while(marking_token_iter != marking_place_iter->tokens.end() && marking_token_iter->getAge() != m_token_iter->getAge()){
									++marking_token_iter;
								}

								if(marking_token_iter == marking_place_iter->tokens.end() || marking_token_iter->getCount() < m_token_iter->getCount()){
									tokensCovered = false;
									break;
								}
							}

							if(!tokensCovered)	break;
						}

						if(tokensCovered){
							return *m_iter;
						}
					}
				}
			}else{
				vector<NonStrictMarking*> coveredMarkings;
				coveredMarkings.push_back(new NonStrictMarking(*marking));
				for(PlaceList::const_iterator p_iter = marking->getPlaceList().begin(); p_iter != marking->getPlaceList().end(); ++p_iter){
					for(TokenList::const_iterator t_iter = p_iter->tokens.begin(); t_iter != p_iter->tokens.end(); ++t_iter){
						for(int i = 1; i <= t_iter->getCount(); ++i){
							vector<NonStrictMarking*> toAdd;
							for(vector<NonStrictMarking*>::iterator iter = coveredMarkings.begin(); iter != coveredMarkings.end(); ++iter){
								NonStrictMarking* new_marking = new NonStrictMarking(**iter);
								for(int ii = i; ii > 0; --ii){
									new_marking->removeToken(p_iter->place->getIndex(), t_iter->getAge());
								}
								toAdd.push_back(new_marking);
							}
							for(vector<NonStrictMarking*>::iterator iter = toAdd.begin(); iter != toAdd.end(); ++iter){
								coveredMarkings.push_back(*iter);
							}
						}
					}
				}

				bool isFirst = true;
				for(vector<NonStrictMarking*>::iterator iter = coveredMarkings.begin(); iter != coveredMarkings.end(); ++iter){
					if(isFirst){
						isFirst = false;
						continue;
					}
					NonStrictMarking* covered = lookup(*iter);
					if(covered != NULL){
						return covered;
					}
					delete *iter;
				}
			}
			return NULL;
		}

        NonStrictMarking* getUnpassed(){
			for(HashMap::iterator hmiter = markings_storage.begin(); hmiter != markings_storage.end(); hmiter++){
				for(NonStrictMarkingList::const_iterator iter = hmiter->second.begin();
						iter != hmiter->second.end();
						iter++){
					if(!(*iter)->meta->passed){
						return *iter;
					}
				}
			}
			return NULL;
		}


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
