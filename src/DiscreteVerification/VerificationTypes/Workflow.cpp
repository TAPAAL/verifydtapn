/*
 * NonStrictSearch.cpp
 *
 *  Created on: 26/04/2012
 *      Author: MathiasGS
 */

#include "Workflow.hpp"

namespace VerifyTAPN {
    namespace DiscreteVerification {

        Workflow::Workflow(TAPN::TimedArcPetriNet& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<NonStrictMarking>* waiting_list)
        : AbstractNaiveVerification<WorkflowPWList>(tapn, initialMarking, query, options, new WorkflowPWList(waiting_list)), in(NULL), out(NULL) {
            for (TimedPlace::Vector::const_iterator iter = tapn.getPlaces().begin(); iter != tapn.getPlaces().end(); iter++) {
                if ((*iter)->getType() == Dead) {
                    (*iter)->setType(Std);
                }
            }
        }

    } /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
