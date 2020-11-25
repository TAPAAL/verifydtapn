/*
 * NonStrictSearch.cpp
 *
 *  Created on: 26/04/2012
 *      Author: MathiasGS
 */

#include "DiscreteVerification/VerificationTypes/WorkflowStrongSoundness.hpp"

#include <utility>

namespace VerifyTAPN { namespace DiscreteVerification {

    WorkflowStrongSoundnessReachability::WorkflowStrongSoundnessReachability(TAPN::TimedArcPetriNet &tapn,
                                                                             NonStrictMarking &initialMarking,
                                                                             AST::Query *query,
                                                                             VerificationOptions options,
                                                                             WaitingList<NonStrictMarking *> *waiting_list)
            : Workflow(tapn, initialMarking, query, std::move(options)), maxValue(-1), outPlace(nullptr) {
        pwList = new WorkflowPWList(waiting_list);
        findInOut();
    }

    WorkflowStrongSoundnessReachability::WorkflowStrongSoundnessReachability(TAPN::TimedArcPetriNet &tapn,
                                                                             NonStrictMarking &initialMarking,
                                                                             AST::Query *query,
                                                                             VerificationOptions options)
            : Workflow(tapn, initialMarking, query, std::move(options)), maxValue(-1), outPlace(nullptr) {
        findInOut();
    }

    void WorkflowStrongSoundnessReachability::findInOut() {
        for (auto* iter : tapn.getPlaces()) {
            if (iter->getTransportArcs().empty() && iter->getInputArcs().empty()) {
                outPlace = iter;
                break;
            }
        }
    }

    WorkflowStrongSoundnessPTrie::WorkflowStrongSoundnessPTrie(
            TAPN::TimedArcPetriNet &tapn,
            NonStrictMarking &initialMarking,
            AST::Query *query,
            VerificationOptions options,
            WaitingList<ptriepointer_t<MetaData *> > *waiting_list) :
            WorkflowStrongSoundnessReachability(
                    tapn,
                    initialMarking,
                    query,
                    std::move(options)) {
        pwList = new WorkflowPWListHybrid(tapn,
                                          waiting_list,
                                          options.getKBound(),
                                          tapn.getNumberOfPlaces(),
                                          tapn.getMaxConstant());
    }

    bool WorkflowStrongSoundnessReachability::run() {

        if (outPlace == nullptr) {
            lastMarking = &initialMarking;
            return true;
        }

        if (handleSuccessor(&initialMarking, nullptr)) {
            return true;
        }

        //Main loop
        while (pwList->hasWaitingStates()) {
            NonStrictMarking *next_marking =
                    static_cast<NonStrictMarking *>(pwList->getNextUnexplored());
            tmpParent = next_marking;

            // push onto trace
            trace.push(next_marking);
            next_marking->meta->inTrace = true;
            validChildren = 0;

            bool noDelay = false;
            auto res = generateAndInsertSuccessors(*next_marking);
            if (res == ADDTOPW_RETURNED_TRUE) {
                clearTrace();
                return true;
            } else if (res == ADDTOPW_RETURNED_FALSE_URGENTENABLED) {
                noDelay = true;
            }

            // Generate delays markings
            if (!noDelay && isDelayPossible(*next_marking)) {
                NonStrictMarking *marking = new NonStrictMarking(*next_marking);
                marking->incrementAge();
                marking->setGeneratedBy(nullptr);

                if (handleSuccessor(marking, next_marking)) {
                    clearTrace();
                    lastMarking = marking;
                    return true;
                }

            }
            if (validChildren != 0) {
                next_marking->setNumberOfChildren(validChildren);
            } else {
                // remove childless markings from stack
                while (!trace.empty() && trace.top()->getNumberOfChildren() <= 1) {
                    trace.top()->meta->inTrace = false;
                    deleteMarking(trace.top());
                    trace.pop();
                }
                if (trace.empty()) {
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
        std::stack<NonStrictMarking *> printStack;
        NonStrictMarking *next = lastMarking;
        do {
            auto *parent = (NonStrictMarking *) next->getParent();
            printStack.push(next);
            next = parent;

        } while (next != nullptr && next->getParent() != nullptr);

        if (next != nullptr && printStack.top() != next) {
            printStack.push(next);
        }

        printXMLTrace(lastMarking, printStack, query, tapn);
    }

    void WorkflowStrongSoundnessPTrie::getTrace() {

        PWListHybrid *pwhlist = dynamic_cast<PWListHybrid *>(this->pwList);

        std::stack<NonStrictMarking *> printStack;

        NonStrictMarking *next = lastMarking;
        if (next != nullptr) {

            printStack.push(next);
            auto *meta =
                    static_cast<MetaDataWithTraceAndEncoding *>(next->meta);

            if (meta == nullptr) {
                // We assume the parent was not deleted on return
                NonStrictMarking *parent = (NonStrictMarking *) lastMarking->getParent();
                if (parent != nullptr)
                    meta =
                            static_cast<MetaDataWithTraceAndEncoding *>(parent->meta);
                delete parent;
            } else {
                meta = meta->parent;
            }

            while (meta != nullptr) {
                next = pwhlist->decode(meta->ep);
                next->setGeneratedBy(meta->generatedBy);
                printStack.top()->setParent(next);
                printStack.push(next);
                meta = meta->parent;
            }
        }
#ifdef CLEANUP
        std::stack < NonStrictMarking*> cleanup = printStack;
#endif
        printXMLTrace(lastMarking, printStack, query, tapn);

#ifdef CLEANUP
        while(!cleanup.empty())
        {
            if(cleanup.top() != lastMarking) // deleted elsewhere
            {
                delete cleanup.top();
                cleanup.pop();
            }
            else
            {
                break;
            }
        }
#endif
    }


    bool WorkflowStrongSoundnessReachability::handleSuccessor(NonStrictMarking *marking, NonStrictMarking *parent) {
        marking->cut(placeStats);
        marking->setParent(parent);

        int totalDelay = marking->calculateTotalDelay();

        unsigned int size = marking->size();

        pwList->setMaxNumTokensIfGreater(size);

        if (size > options.getKBound()) {
            delete marking;
            return false;
        }

        /* Handle max */
        // Map to existing marking if any
        NonStrictMarking *old = (NonStrictMarking *) pwList->addToPassed(marking, true);

        if (old != nullptr) {
            if (old->meta->totalDelay < totalDelay) {
                if (old->meta->inTrace) {
                    // delay loop
                    lastMarking = marking;
                    // make sure we can print trace
                    marking->setNumberOfChildren(1);
                    maxValue = totalDelay;
                    deleteMarking(old);
                    return true;
                } else {
                    // search again to find maxdelay
                    swapData(marking, old);
                    old->meta->totalDelay = totalDelay;
                    delete marking;
                    marking = old;
                    // fall through on purpose
                }
            } else {
                deleteMarking(old);
                delete marking;
                // already seen this maxage/marking combination
                return false;
            }
        } else {
            marking->meta->totalDelay = totalDelay;
        }

        // if we reached bound
        if (marking->meta->totalDelay > options.getWorkflowBound()) return true;


        if (marking->numberOfTokensInPlace(outPlace->getIndex()) == 0) {
            // if nonterminal, add to waiting
            pwList->addLastToWaiting();
            ++validChildren;
        } else {
            // if terminal, update max_value and last marking of trace
            if (maxValue < marking->meta->totalDelay) {
                maxValue = marking->meta->totalDelay;
                if (lastMarking != nullptr) deleteMarking(lastMarking);
                lastMarking = marking;
                return false;
            }
        }

        deleteMarking(marking);

        return false;
    }

    void WorkflowStrongSoundnessReachability::swapData(NonStrictMarking *marking, NonStrictMarking *old) {
        old->setGeneratedBy(marking->getGeneratedBy());
        old->setParent(marking->getParent());
    }

    void WorkflowStrongSoundnessPTrie::swapData(NonStrictMarking *marking, NonStrictMarking *parent) {
        PWListHybrid *pwhlist = dynamic_cast<PWListHybrid *>(this->pwList);
        auto *meta =
                static_cast<MetaDataWithTraceAndEncoding *>(parent->meta);

        meta->generatedBy = marking->getGeneratedBy();
        meta->parent = pwhlist->parent;
    }

    void WorkflowStrongSoundnessPTrie::clearTrace() {
        if (!trace.empty()) trace.pop(); // pop parent, used in getTrace
        while (!trace.empty()) {
            delete trace.top();
            trace.pop();
        }
    }

} } /* namespace VerifyTAPN */
