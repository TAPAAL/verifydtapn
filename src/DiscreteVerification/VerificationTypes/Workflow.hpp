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
#include "AbstractNaiveVerification.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

template<typename T>
class Workflow : public AbstractNaiveVerification<WorkflowPWList,T> {
public:    
	Workflow(TAPN::TimedArcPetriNet& tapn, T& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<NonStrictMarking*>* waiting_list);
	virtual void printExecutionTime(ostream& stream) = 0;
        
protected:
    void deleteMarking(NonStrictMarking*){};
	TimedPlace* in;
	TimedPlace* out;
};

template<typename T>
Workflow<T>::Workflow(TAPN::TimedArcPetriNet& tapn, T& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<NonStrictMarking*>* waiting_list)
: AbstractNaiveVerification<WorkflowPWList,T>(tapn, initialMarking, query, options, new WorkflowPWList(waiting_list)), in(NULL), out(NULL){
    for (TimedPlace::Vector::const_iterator iter = tapn.getPlaces().begin(); iter != tapn.getPlaces().end(); iter++) {
        if ((*iter)->getType() == Dead) {
            (*iter)->setType(Std);
        }
    }
}

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* NONSTRICTSEARCH_HPP_ */