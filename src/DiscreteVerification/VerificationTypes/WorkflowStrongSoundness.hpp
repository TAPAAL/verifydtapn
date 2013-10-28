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
	WorkflowStrongSoundnessReachability(TAPN::TimedArcPetriNet& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<NonStrictMarking>* waiting_list);

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
	int max_value;
	TimedPlace* timer;
	TimedPlace* term1;
	TimedPlace* term2;
};

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* NONSTRICTSEARCH_HPP_ */
