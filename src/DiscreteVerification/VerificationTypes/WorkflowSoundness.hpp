/*
 * NonStrictSearch.hpp
 *
 *  Created on: 26/04/2012
 *      Author: MathiasGS
 */

#ifndef WORKFLOWSOUNDNESS_HPP_
#define WORKFLOWSOUNDNESS_HPP_

#include "../DataStructures/WorkflowPWList.hpp"
#include "../../Core/TAPN/TAPN.hpp"
#include "../../Core/QueryParser/AST.hpp"
#include "../../Core/VerificationOptions.hpp"
#include "../../Core/TAPN/TimedPlace.hpp"
#include "../../Core/TAPN/TimedTransition.hpp"
#include "../../Core/TAPN/TimedInputArc.hpp"
#include "../../Core/TAPN/TransportArc.hpp"
#include "../../Core/TAPN/InhibitorArc.hpp"
#include "../../Core/TAPN/OutputArc.hpp"
#include "../SuccessorGenerator.hpp"
#include "../QueryVisitor.hpp"
#include "../DataStructures/NonStrictMarking.hpp"
#include <stack>
#include "ReachabilitySearch.hpp"
#include "../DataStructures/WaitingList.hpp"
#include "Workflow.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

class WorkflowSoundness : public Workflow<NonStrictMarking> {
public:
        enum ModelType{
		MTAWFN, ETAWFN, NOTTAWFN
	};
        
	WorkflowSoundness(TAPN::TimedArcPetriNet& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<NonStrictMarking>* waiting_list);
	virtual ~WorkflowSoundness();
	bool verify();
	virtual void getTrace(){
		return getTrace(lastMarking);
	}
	void printExecutionTime(ostream& stream){
		stream << "Minimum execution time: " << minExec << endl;
	}
	void printMessages(ostream& stream){
		if(coveredMarking != NULL){
			stream << "Covered marking: " << *coveredMarking << endl;
			getTrace(coveredMarking);
		}
	}
        inline const ModelType getModelType() const{ return modelType; }
protected:
	bool addToPW(NonStrictMarking* marking, NonStrictMarking* parent);
	bool checkForCoveredMarking(NonStrictMarking* marking);
	void getTrace(NonStrictMarking* base);
        ModelType calculateModelType();
        
protected:
    stack<WorkflowSoundnessMetaData*> passedStack;
    int minExec;
    unsigned int linearSweepTreshold;
    NonStrictMarking* coveredMarking;
    ModelType modelType;

};

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* NONSTRICTSEARCH_HPP_ */
