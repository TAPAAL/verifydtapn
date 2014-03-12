/*
 * NonStrictSearch.cpp
 *
 *  Created on: 26/04/2012
 *      Author: MathiasGS
 */

#include "WorkflowStrongSoundness.hpp"

namespace VerifyTAPN {
    namespace DiscreteVerification {

        WorkflowStrongSoundnessReachability::WorkflowStrongSoundnessReachability(TAPN::TimedArcPetriNet& tapn, NonStrictMarkingWithDelay& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<NonStrictMarking>* waiting_list)
        : Workflow<NonStrictMarkingWithDelay>(tapn, initialMarking, query, options, waiting_list), max_value(-1), outPlace(NULL){
            // Find timer place and store as out
            for (TimedPlace::Vector::const_iterator iter = tapn.getPlaces().begin(); iter != tapn.getPlaces().end(); ++iter) {
                if ((*iter)->getTransportArcs().empty() && (*iter)->getInputArcs().empty()) {
                    outPlace = *iter;
                    break;
                }
            }
        };

        bool WorkflowStrongSoundnessReachability::verify() {
            if (addToPW(&initialMarking, NULL)) {
                return true;
            }

            //Main loop
            while (pwList->hasWaitingStates()) {
                NonStrictMarkingWithDelay& next_marking = 
                        static_cast<NonStrictMarkingWithDelay&>(*pwList->getNextUnexplored());
                tmpParent = &next_marking;

                bool noDelay = false;
                Result res = successorGenerator.generateAndInsertSuccessors(next_marking);
                if (res == QUERY_SATISFIED) {
                    return true;
                } else if (res == URGENT_ENABLED) {
                    noDelay = true;
                }

                // Generate next markings
                if (!noDelay && isDelayPossible(next_marking)) {
                    NonStrictMarkingWithDelay* marking = new NonStrictMarkingWithDelay(next_marking);
                    marking->incrementAge();
                    marking->setGeneratedBy(NULL);
                    marking->setTotalDelay(next_marking.getTotalDelay()+1);
                    if(marking->getTotalDelay() > options.getWorkflowBound()){
                        marking->setParent(&next_marking);
                        return true;
                    }
                    if (addToPW(marking, &next_marking)) {
                        return true;
                    }
                }
            }
            return false;
        }

        void WorkflowStrongSoundnessReachability::getTrace() {
            std::stack < NonStrictMarkingWithDelay*> printStack;
            NonStrictMarkingWithDelay* next = lastMarking;
            do {
                NonStrictMarkingWithDelay* parent = (NonStrictMarkingWithDelay*)next->getParent();
                printStack.push(next);
                next = parent;

            } while (next != NULL && next->getParent() != NULL);

            if (printStack.top() != next) {
                printStack.push(next);
            }

            if (options.getXmlTrace()) {
                printXMLTrace(lastMarking, printStack, query, tapn);
            } else {
                printHumanTrace(lastMarking, printStack, query->getQuantifier());
            }
        }

        bool WorkflowStrongSoundnessReachability::addToPW(NonStrictMarkingWithDelay* marking, NonStrictMarkingWithDelay* parent) {
            marking->cut();
            marking->setParent(parent);

            unsigned int size = marking->size();

            pwList->setMaxNumTokensIfGreater(size);

            if (size > options.getKBound()) {
                delete marking;
                return false;
            }

            /* Handle max */
            // Map to existing marking if any
            NonStrictMarkingWithDelay* old = (NonStrictMarkingWithDelay*)pwList->addToPassed(marking);
            bool isNew = false;
            if(old == NULL){
                    isNew = true;
            }

            
            if(marking->getTotalDelay() > max_value){
                max_value = marking->getTotalDelay();
                lastMarking = marking;
            }
            if (isNew && marking->numberOfTokensInPlace(outPlace->getIndex()) == 0) {
                pwList->addToWaiting(marking);
            }
            return false;
        }

    } /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
