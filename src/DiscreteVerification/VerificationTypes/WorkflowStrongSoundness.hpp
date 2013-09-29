/*
 * NonStrictSearch.hpp
 *
 *  Created on: 26/04/2012
 *      Author: MathiasGS
 */

#ifndef WORKFLOWSTRONGSOUNDNESS_HPP_
#define WORKFLOWSTRONGSOUNDNESS_HPP_

#include "Workflow.hpp"
#include <stack>

namespace VerifyTAPN {
namespace DiscreteVerification {

class WorkflowStrongSoundnessReachability : public Workflow{
public:
	WorkflowStrongSoundnessReachability(TAPN::TimedArcPetriNet& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<NonStrictMarking>* waiting_list)
    : Workflow(tapn,initialMarking, query, options, waiting_list), pwList(new WorkflowPWList(waiting_list)), max_value(-1), timer(NULL), term1(NULL), term2(NULL)
    {
		// Find timer place and store as out
		for(TimedPlace::Vector::const_iterator iter = tapn.getPlaces().begin(); iter != tapn.getPlaces().end(); ++iter){
			if((*iter)->getInvariant() != (*iter)->getInvariant().LS_INF){
				if(timer == NULL || timer->getInvariant().getBound() < (*iter)->getInvariant().getBound()){
					timer = *iter;
				}
			}

			if(!(*iter)->getTransportArcs().empty() || !(*iter)->getInputArcs().empty()){
				continue;
			}

			if(term1 == NULL){
				term1 = *iter;
			}else if(term2 == NULL){
				term2 = *iter;
			}else{
				assert(false);
			}
		}
    };

	bool verify();
	void getTrace();

	void printExecutionTime(ostream& stream){
		stream << "Maximum execution time: " << max_value << endl;
	}

	void printStats(){
			std::cout << "  discovered markings:\t" << pwList->discoveredMarkings << std::endl;
			std::cout << "  explored markings:\t" << pwList->size()-pwList->explored() << std::endl;
			std::cout << "  stored markings:\t" << pwList->size() << std::endl;
		}

	inline unsigned int maxUsedTokens(){ return pwList->maxNumTokensInAnyMarking; };

protected:
	virtual bool addToPW(NonStrictMarking* m){
			return addToPW(m, tmpParent);
		};

	bool addToPW(NonStrictMarking* marking, NonStrictMarking* parent);
protected:
	PWListBase* pwList;
	int max_value;
	TimedPlace* timer;
	TimedPlace* term1;
	TimedPlace* term2;
};

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* NONSTRICTSEARCH_HPP_ */
