/*
 * NonStrictSearch.hpp
 *
 *  Created on: 26/04/2012
 *      Author: MathiasGS
 */

#ifndef WORKFLOWSTRONGSOUNDNESS_HPP_
#define WORKFLOWSTRONGSOUNDNESS_HPP_

#include "ReachabilitySearch.hpp"
#include "../DataStructures/WorkflowPWList.hpp"
#include "Workflow.hpp"
#include <stack>

namespace VerifyTAPN {
namespace DiscreteVerification {

class WorkflowStrongSoundnessReachability : public Workflow<NonStrictMarkingWithDelay>{
public:

	WorkflowStrongSoundnessReachability(TAPN::TimedArcPetriNet& tapn, NonStrictMarkingWithDelay& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<NonStrictMarking>* waiting_list);

	bool verify();
	void getTrace();

	void printExecutionTime(ostream& stream){
		stream << "Maximum execution time: " << max_value << endl;
	}
        

protected:
	bool addToPW(NonStrictMarkingWithDelay* marking, NonStrictMarkingWithDelay* parent);
protected:
	int max_value;
	TimedPlace* outPlace;
        int validChildren;
};

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* NONSTRICTSEARCH_HPP_ */
