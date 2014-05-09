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
#include <algorithm>

namespace VerifyTAPN {
namespace DiscreteVerification {

class WorkflowStrongSoundnessReachability : public Workflow<NonStrictMarkingWithTotalDelay>{
public:

	WorkflowStrongSoundnessReachability(TAPN::TimedArcPetriNet& tapn, NonStrictMarkingWithTotalDelay& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<NonStrictMarking>* waiting_list);

	bool verify();
	void getTrace();

	void printExecutionTime(ostream& stream){
		stream << "Maximum execution time: " << maxValue << endl;
	}
        

protected:
	bool addToPW(NonStrictMarkingWithTotalDelay* marking, NonStrictMarkingWithTotalDelay* parent);
protected:
	int maxValue;
	TimedPlace* outPlace;
        int validChildren;
};

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* NONSTRICTSEARCH_HPP_ */
