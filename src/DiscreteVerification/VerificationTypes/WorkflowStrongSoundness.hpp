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

class WorkflowStrongSoundnessReachability : public Workflow{
public:

	WorkflowStrongSoundnessReachability(TAPN::TimedArcPetriNet& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<NonStrictMarking*>* waiting_list);

        WorkflowStrongSoundnessReachability(TAPN::TimedArcPetriNet& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options);
        
	bool verify();
	virtual void getTrace();

	void printExecutionTime(ostream& stream){
		stream << "Maximum execution time: " << (maxValue * tapn.getGCD()) << endl;
	}
        

protected:
        void findInOut();
	bool addToPW(NonStrictMarking* marking, NonStrictMarking* parent);
        virtual void swapData(NonStrictMarking* marking, NonStrictMarking* old);

protected:
	int maxValue;
	TimedPlace* outPlace;
        int validChildren;
};

class WorkflowStrongSoundnessPTrie : public WorkflowStrongSoundnessReachability
{
public:
    WorkflowStrongSoundnessPTrie(
            TAPN::TimedArcPetriNet& tapn,
            NonStrictMarking& initialMarking, 
            AST::Query* query, 
            VerificationOptions options, 
            WaitingList<ptriepointer_t<MetaData*> >* waiting_list);
    virtual void getTrace();
    virtual void deleteMarking(NonStrictMarking* marking)
    {
        delete marking;
    }

protected:
        virtual void swapData(NonStrictMarking* marking, NonStrictMarking* old);
};

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* NONSTRICTSEARCH_HPP_ */
