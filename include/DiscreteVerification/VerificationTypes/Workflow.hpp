/*
 * NonStrictSearch.hpp
 *
 *  Created on: 26/04/2012
 *      Author: MathiasGS
 */

#ifndef WORKFLOW_HPP_
#define WORKFLOW_HPP_

#include "DiscreteVerification/DataStructures/WorkflowPWList.hpp"
#include "Core/TAPN/TAPN.hpp"
#include "Core/QueryParser/AST.hpp"
#include "Core/VerificationOptions.hpp"
#include "Core/TAPN/TimedPlace.hpp"
#include "Core/TAPN/TimedTransition.hpp"
#include "Core/TAPN/TimedInputArc.hpp"
#include "Core/TAPN/TransportArc.hpp"
#include "Core/TAPN/InhibitorArc.hpp"
#include "Core/TAPN/OutputArc.hpp"
#include "DiscreteVerification/QueryVisitor.hpp"
#include "DiscreteVerification/DataStructures/NonStrictMarking.hpp"
#include "DiscreteVerification/DataStructures/WaitingList.hpp"
#include "AbstractNaiveVerification.hpp"
#include "Core/TAPN/TAPN.hpp"

#include <stack>

namespace VerifyTAPN { namespace DiscreteVerification {

    using namespace TAPN;

    class Workflow : public AbstractNaiveVerification<WorkflowPWListBasic, NonStrictMarking, Generator> {
    public:
        Workflow(TAPN::TimedArcPetriNet &tapn, NonStrictMarking &initialMarking, AST::Query *query,
                 const VerificationOptions &options)
                : AbstractNaiveVerification<WorkflowPWListBasic, NonStrictMarking, Generator>(tapn, initialMarking,
                                                                                              query, options, nullptr),
                  in(nullptr), out(nullptr) {

            for (auto* p : tapn.getPlaces()) {
                if (p->getType() == Dead) {
                    p->setType(Std);
                }
            }
        }

        virtual void printExecutionTime(std::ostream &stream) = 0;

    protected:
        virtual void deleteMarking(NonStrictMarking *marking) {

        };
        TimedPlace *in;
        TimedPlace *out;
    };

} } /* namespace VerifyTAPN */
#endif /* NONSTRICTSEARCH_HPP_ */
