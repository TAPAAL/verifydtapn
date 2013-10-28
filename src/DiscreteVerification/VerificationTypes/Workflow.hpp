/*
 * NonStrictSearch.hpp
 *
 *  Created on: 26/04/2012
 *      Author: MathiasGS
 */

#ifndef WORKFLOW_HPP_
#define WORKFLOW_HPP_

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

namespace VerifyTAPN {
namespace DiscreteVerification {

class Workflow : public Verification<NonStrictMarking> {
public:
    	enum ModelType{
		MTAWFN, ETAWFN, NOTTAWFN
	};
    
	Workflow(TAPN::TimedArcPetriNet& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<NonStrictMarking>* waiting_list);

	inline const ModelType getModelType() const{ return modelType; }
        void printTransitionStatistics() const { successorGenerator.printTransitionStatistics(std::cout); }
        bool isDelayPossible(NonStrictMarking& marking);
	ModelType calculateModelType();

	virtual void printExecutionTime(ostream& stream){}
	virtual void printMessages(ostream& stream){}

protected:
    	TAPN::TimedArcPetriNet& tapn;
	NonStrictMarking& initialMarking;
	AST::Query* query;
	VerificationOptions options;
	TimedPlace* in;
	TimedPlace* out;
	ModelType modelType;
        WorkflowPWList* pwList;
        SuccessorGenerator<NonStrictMarking> successorGenerator;
	NonStrictMarking* lastMarking;
        NonStrictMarking* tmpParent; 
};

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* NONSTRICTSEARCH_HPP_ */
