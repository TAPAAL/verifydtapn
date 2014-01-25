/*
 * NonStrictSearch.cpp
 *
 *  Created on: 26/04/2012
 *      Author: MathiasGS
 */

#include "WorkflowStrongSoundness.hpp"

namespace VerifyTAPN {
    namespace DiscreteVerification {

        WorkflowStrongSoundnessReachability::WorkflowStrongSoundnessReachability(TAPN::TimedArcPetriNet& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<NonStrictMarking>* waiting_list)
        : Workflow(tapn, initialMarking, query, options, waiting_list), max_value(-1), timer(NULL), term1(NULL), term2(NULL) {
            // Find timer place and store as out
            for (TimedPlace::Vector::const_iterator iter = tapn.getPlaces().begin(); iter != tapn.getPlaces().end(); ++iter) {
                if ((*iter)->getInvariant() != (*iter)->getInvariant().LS_INF) {
                    if (timer == NULL || timer->getInvariant().getBound() < (*iter)->getInvariant().getBound()) {
                        timer = *iter;
                    }
                }

                if (!(*iter)->getTransportArcs().empty() || !(*iter)->getInputArcs().empty()) {
                    continue;
                }

                if (term1 == NULL) {
                    term1 = *iter;
                } else if (term2 == NULL) {
                    term2 = *iter;
                } else {
                    assert(false);
                }
            }
        };

        bool WorkflowStrongSoundnessReachability::verify() {
            if (addToPW(&initialMarking, NULL)) {
                return true;
            }

            //Main loop
            while (pwList->hasWaitingStates()) {
                NonStrictMarking& next_marking = *pwList->getNextUnexplored();
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
                    NonStrictMarking* marking = new NonStrictMarking(next_marking);
                    marking->incrementAge();
                    marking->setGeneratedBy(NULL);
                    if (addToPW(marking, &next_marking)) {
                        return true;
                    }
                }
            }

            return false;
        }

        void WorkflowStrongSoundnessReachability::getTrace() {
            std::stack < NonStrictMarking*> printStack;
            NonStrictMarking* next = lastMarking;
            do {
                NonStrictMarking* parent = ((WorkflowStrongSoundnessMetaData*) next->meta)->parent;
                printStack.push(next);
                next = parent;

            } while (((WorkflowStrongSoundnessMetaData*) next->meta)->parent != NULL);

            if (printStack.top() != next) {
                printStack.push(next);
            }

            if (options.getXmlTrace()) {
                printXMLTrace(lastMarking, printStack, query, tapn);
            } else {
                printHumanTrace(lastMarking, printStack, query->getQuantifier());
            }
        }

        bool WorkflowStrongSoundnessReachability::addToPW(NonStrictMarking* marking, NonStrictMarking* parent) {
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
            NonStrictMarking* old = pwList->addToPassed(marking);
            bool isNew = false;
            if(old == NULL){
                    isNew = true;
                    marking->meta = new WorkflowSoundnessMetaData();
            } else  {
                delete marking;
                marking = old;
            }

            WorkflowStrongSoundnessMetaData* meta = (WorkflowStrongSoundnessMetaData*) marking->meta;

            if (parent != NULL) meta->parent = parent;

            if (!marking->getTokenList(timer->getIndex()).empty() &&
                    (marking->getTokenList(timer->getIndex()).at(0).getAge() > max_value ||
                    (marking->getTokenList(timer->getIndex()).at(0).getAge() == max_value &&
                    (!marking->getTokenList(term1->getIndex()).empty() || !marking->getTokenList(term2->getIndex()).empty())))) {
                max_value = marking->getTokenList(timer->getIndex()).at(0).getAge();
                lastMarking = marking;
            }

            // Add to passed
            if (isNew) {
                QueryVisitor<NonStrictMarking> checker(*marking, tapn);
                BoolResult context;

                query->accept(checker, context);
                if (context.value) {
                    lastMarking = marking;
                    return true;
                } else {
                    return false;
                }
            }

            return false;
        }

    } /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
