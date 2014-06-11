/*
 * NonStrictSearch.cpp
 *
 *  Created on: 26/04/2012
 *      Author: MathiasGS
 */

#include "WorkflowStrongSoundness.hpp"

namespace VerifyTAPN {
    namespace DiscreteVerification {

        WorkflowStrongSoundnessReachability::WorkflowStrongSoundnessReachability(TAPN::TimedArcPetriNet& tapn, NonStrictMarkingWithTotalDelay& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<NonStrictMarking>* waiting_list)
        : Workflow<NonStrictMarkingWithTotalDelay>(tapn, initialMarking, query, options, waiting_list), maxValue(-1), outPlace(NULL){
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
                NonStrictMarkingWithTotalDelay& next_marking = 
                        static_cast<NonStrictMarkingWithTotalDelay&>(*pwList->getNextUnexplored());
                tmpParent = &next_marking;
                
                // push onto trace
                trace.push(&next_marking);
                next_marking.meta->inTrace = true;
                validChildren = 0;

                bool noDelay = false;
                Result res = successorGenerator.generateAndInsertSuccessors(next_marking);
                if (res == ADDTOPW_RETURNED_TRUE) {
                    return true;
                } else if (res == ADDTOPW_RETURNED_FALSE_URGENTENABLED) {
                    noDelay = true;
                }

                // Generate delays markings
                if (!noDelay && isDelayPossible(next_marking)) {
                    NonStrictMarkingWithTotalDelay* marking = new NonStrictMarkingWithTotalDelay(next_marking);
                    marking->incrementAge();
                    marking->setGeneratedBy(NULL);
                    if(marking->getTotalDelay() > options.getWorkflowBound()){
                        // if the bound is exceeded, terminate
                        marking->setParent(&next_marking);
                        lastMarking = marking;
                        return true;
                    }
                    if (addToPW(marking, &next_marking)) {
                        lastMarking = marking;
                        return true;
                    }
                }
                if(validChildren != 0){
                    next_marking.setNumberOfChildren(validChildren);
                } else {
                    // remove childless markings from stack
                    while(!trace.empty() && trace.top()->getNumberOfChildren() <= 1){
                            trace.top()->meta->inTrace = false;
                            trace.pop();
                    }
                    if(trace.empty()){
                        // this should only happen when the waitinglist is empty
                        return false;
                    }
                    trace.top()->decrementNumberOfChildren();
                }
            }
            // should never reach here
            assert(false);
            return false;
        }

        void WorkflowStrongSoundnessReachability::getTrace() {
            std::stack < NonStrictMarkingWithTotalDelay*> printStack;
            NonStrictMarkingWithTotalDelay* next = lastMarking;
            do {
                NonStrictMarkingWithTotalDelay* parent = (NonStrictMarkingWithTotalDelay*)next->getParent();
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

        bool WorkflowStrongSoundnessReachability::addToPW(NonStrictMarkingWithTotalDelay* marking, NonStrictMarkingWithTotalDelay* parent) {
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
            NonStrictMarkingWithTotalDelay* old = (NonStrictMarkingWithTotalDelay*)pwList->addToPassed(marking);
            if(old != NULL) {
                if(old->getTotalDelay() < marking->getTotalDelay()) {
                    if(old->meta->inTrace){
                        // delay loop
                        lastMarking = marking;
                        // make sure we can print trace
                        marking->setNumberOfChildren(1);
                        trace.push(marking);
                        maxValue = marking->getTotalDelay();
                        return true;
                    } else {
                        // search again to find maxdelay
                        // copy data from new
                        old->setParent(marking->getParent());
                        old->setGeneratedBy(marking->getGeneratedBy());
                        old->setTotalDelay(marking->getTotalDelay());
                        delete marking;
                        marking = old;
                        // fall through on purpose
                    }
                } else {
                    delete marking;
                    // already seen this maxage/marking combination
                    return false;
                }
            } else {
                marking->meta = new MetaData();
            }
            
            if(marking->numberOfTokensInPlace(outPlace->getIndex()) == 0){
                // if nonterminal, add to waiting
                pwList->addToWaiting(marking);
                ++validChildren;
            } else {
                // if terminal, update max_value and last marking of trace
                if(maxValue < marking->getTotalDelay()) {
                    maxValue = marking->getTotalDelay();
                    lastMarking = marking;
                }
            }
            return false;
        }

    } /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */