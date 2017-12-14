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
#include "../QueryVisitor.hpp"
#include "../DataStructures/NonStrictMarking.hpp"
#include <stack>
#include "ReachabilitySearch.hpp"
#include "../DataStructures/WaitingList.hpp"
#include "AbstractNaiveVerification.hpp"
#include "../../Core/TAPN/TAPN.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

    using namespace TAPN;

class Workflow : public AbstractNaiveVerification<WorkflowPWListBasic,NonStrictMarking> {
public:    
	Workflow(TAPN::TimedArcPetriNet& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options)
        : AbstractNaiveVerification<WorkflowPWListBasic,NonStrictMarking>(tapn, initialMarking, query, options, NULL), in(NULL), out(NULL){
    
            for (TimedPlace::Vector::const_iterator iter = tapn.getPlaces().begin(); 
                    iter != tapn.getPlaces().end(); iter++) {
                if ((*iter)->getType() == Dead) {
                    (*iter)->setType(Std);
                }
            }
        }
	virtual void printExecutionTime(ostream& stream) = 0;
        
protected:
    virtual void deleteMarking(NonStrictMarking* marking){

    };
	TimedPlace* in;
	TimedPlace* out;
};

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* NONSTRICTSEARCH_HPP_ */