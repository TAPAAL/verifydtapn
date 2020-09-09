/*
 * NonStrictSearch.cpp
 *
 *  Created on: 26/04/2012
 *      Author: MathiasGS
 */

#include "DiscreteVerification/VerificationTypes/WorkflowSoundness.hpp"
#include <limits>
#include <utility>

namespace VerifyTAPN::DiscreteVerification {

    WorkflowSoundness::WorkflowSoundness(TAPN::TimedArcPetriNet &tapn, NonStrictMarking &initialMarking,
                                         AST::Query *query, VerificationOptions options,
                                         WaitingList<NonStrictMarking *> *waiting_list)
            : Workflow(tapn, initialMarking, query, std::move(options)), passedStack(), minExec(INT_MAX),
              linearSweepTreshold(3), coveredMarking(nullptr), modelType(calculateModelType()) {
        pwList = new WorkflowPWList(waiting_list);

    }


    WorkflowSoundness::WorkflowSoundness(TAPN::TimedArcPetriNet &tapn, NonStrictMarking &initialMarking,
                                         AST::Query *query, VerificationOptions options)
            : Workflow(tapn, initialMarking, query, std::move(options)), passedStack(), minExec(INT_MAX),
              linearSweepTreshold(3), coveredMarking(nullptr), modelType(calculateModelType()) {

    }

    WorkflowSoundnessPTrie::WorkflowSoundnessPTrie(TAPN::TimedArcPetriNet &tapn, NonStrictMarking &initialMarking,
                                                   AST::Query *query, const VerificationOptions &options,
                                                   WaitingList<ptriepointer_t<MetaData *> > *waiting_list)
            : WorkflowSoundness(tapn, initialMarking, query, options) {
        int kbound = modelType == MTAWFN ? (std::numeric_limits<int>::max() - 1) : options.getKBound();
        pwList = new WorkflowPWListHybrid(tapn,
                                          waiting_list,
                                          kbound,
                                          tapn.getNumberOfPlaces(),
                                          tapn.getMaxConstant());

    }


    bool WorkflowSoundness::run() {
        if (handleSuccessor(&initialMarking, NULL)) {
            return false;
        }

        // Phase 1
        while (pwList->hasWaitingStates()) {

            NonStrictMarking *next_marking = pwList->getNextUnexplored();
            tmpParent = next_marking;
            bool noDelay = false;

            auto res = generateAndInsertSuccessors(*next_marking);
            if (res == ADDTOPW_RETURNED_TRUE) {
                return false;
            } else if (res == ADDTOPW_RETURNED_FALSE_URGENTENABLED) {
                noDelay = true;
            }

            // Generate next markings
            if (!noDelay && isDelayPossible(*next_marking)) {
                NonStrictMarking *marking = new NonStrictMarking(*next_marking);
                marking->incrementAge();
                marking->setGeneratedBy(NULL);
                if (handleSuccessor(marking, next_marking)) {
                    return false;
                }
            }
            deleteMarking(next_marking);
        }

        // Phase 2
        // mark all as passed which ends in a final marking
        int passed = numberOfPassed();

        if (passed < pwList->stored) {
            lastMarking = pwList->getUnpassed();
            return false;
        } else {
            return true;
        }
    }

    int WorkflowSoundness::numberOfPassed() {
        int passed = 0;
        while (passedStack.size()) {
            WorkflowSoundnessMetaData *next =
                    static_cast<WorkflowSoundnessMetaData *>(passedStack.top());
            passedStack.pop();
            if (next->passed) continue;
            next->passed = true;
            ++passed;
            for (vector<MetaData *>::iterator iter = next->parents.begin();
                 iter != next->parents.end(); iter++) {
                passedStack.push(*iter);
            }
        }
        return passed;
    }

    int WorkflowSoundnessPTrie::numberOfPassed() {
        int passed = 0;
        while (passedStack.size()) {
            WorkflowSoundnessMetaDataWithEncoding *next =
                    static_cast<WorkflowSoundnessMetaDataWithEncoding *>(passedStack.top());

            passedStack.pop();
            if (next->passed) continue;
            next->passed = true;
            ++passed;
            for (vector<MetaData *>::iterator iter = next->parents.begin();
                 iter != next->parents.end(); iter++) {
                passedStack.push(*iter);
            }
        }
        return passed;
    }


    bool WorkflowSoundness::handleSuccessor(NonStrictMarking *marking, NonStrictMarking *parent) {
        marking->cut(placeStats);

        unsigned int size = marking->size();

        // Check K-bound
        pwList->setMaxNumTokensIfGreater(size);
        if (modelType == ETAWFN && size > options.getKBound()) {
            if (lastMarking != NULL) deleteMarking(lastMarking);
            lastMarking = marking;
            setMetaParent(lastMarking);
            return true;    // Terminate false
        }

        // Map to existing marking if any
        bool isNew = false;
        marking->setParent(parent);
        NonStrictMarking *old = pwList->addToPassed(marking, false);
        if (old == nullptr) {
            isNew = true;
        } else {
            delete marking;
            marking = old;
        }

        // add to parents_set
        if (parent != nullptr) {
            addParentMeta(marking->meta, parent->meta);
            if (marking->getGeneratedBy() == nullptr) {
                marking->meta->totalDelay = min(marking->meta->totalDelay,
                                                parent->meta->totalDelay + 1);    // Delay
            } else {
                marking->meta->totalDelay = min(marking->meta->totalDelay,
                                                parent->meta->totalDelay);    // Transition
            }
        } else {
            marking->meta->totalDelay = 0;
        }



        // Test if final place
        if (marking->numberOfTokensInPlace(out->getIndex()) > 0) {
            if (size == 1) {
                passedStack.push(marking->meta);
                // Set min
                marking->meta->totalDelay = min(marking->meta->totalDelay,
                                                parent->meta->totalDelay);    // Transition
                // keep track of shortest trace
                if (marking->meta->totalDelay < minExec) {
                    if (lastMarking != NULL) deleteMarking(lastMarking);

                    minExec = marking->meta->totalDelay;
                    lastMarking = marking;
                    return false;
                }
            } else {
                if (lastMarking != NULL) deleteMarking(lastMarking);
                lastMarking = marking;
                return true;    // Terminate false
            }
        } else {
            // If new marking
            if (isNew) {
                pwList->addLastToWaiting();
                if (parent != NULL && marking->canDeadlock(tapn, 0)) {
                    if (lastMarking != NULL) deleteMarking(lastMarking);
                    lastMarking = marking;
                    return true;
                }
                if (modelType == MTAWFN && checkForCoveredMarking(marking)) {
                    if (lastMarking != NULL) deleteMarking(lastMarking);
                    lastMarking = marking;
                    return true;    // Terminate false
                }
            }
        }
        deleteMarking(marking);
        return false;
    }

    void WorkflowSoundness::addParentMeta(MetaData *meta, MetaData *parent) {
        WorkflowSoundnessMetaData *markingmeta = ((WorkflowSoundnessMetaData *) meta);
        markingmeta->parents.push_back(parent);

    }

    void WorkflowSoundnessPTrie::addParentMeta(MetaData *meta, MetaData *parent) {
        assert(meta != nullptr);
        assert(parent != nullptr);
        WorkflowSoundnessMetaDataWithEncoding *markingmeta = ((WorkflowSoundnessMetaDataWithEncoding *) meta);
        markingmeta->parents.push_back(parent);

    }

    bool WorkflowSoundness::checkForCoveredMarking(NonStrictMarking *marking) {
        if (marking->size() <= options.getKBound()) {
            return false;    // Do not run check on small markings (invoke more rarely)
        }

        NonStrictMarking *covered = pwList->getCoveredMarking(marking, (marking->size() > linearSweepTreshold));
        if (covered != nullptr) {
            coveredMarking = covered;
            return true;
        }

        return false;
    }

    void WorkflowSoundness::getTrace(NonStrictMarking *marking) {

        stack<NonStrictMarking *> printStack;
        NonStrictMarking *next = marking;
        if (next != nullptr) {
            do {
                printStack.push(next);
                next = (NonStrictMarking *) next->getParent();
            } while (next);
        }

        printXMLTrace(marking, printStack, query, tapn);

    }


    void WorkflowSoundnessPTrie::getTrace(NonStrictMarking *marking) {

        PWListHybrid *pwhlist = dynamic_cast<PWListHybrid *>(this->pwList);

        stack<NonStrictMarking *> printStack;

        if (marking != nullptr) {
            printStack.push(marking);

            auto *meta =
                    static_cast<MetaDataWithTraceAndEncoding *>(marking->meta);
            if (meta) {
                marking->setGeneratedBy(meta->generatedBy);

                meta = meta->parent;
                while (meta != nullptr) {
                    NonStrictMarking *next = pwhlist->decode(meta->ep);
                    printStack.push(next);
                    next->setGeneratedBy(meta->generatedBy);
                    printStack.top()->setParent(next);
                    meta = meta->parent;
                }
            }
            printStack.top()->setGeneratedBy(NULL);
        }

#ifdef CLEANUP
        stack < NonStrictMarking*> clearStack = printStack;
#endif
        printXMLTrace(marking, printStack, query, tapn);
#ifdef CLEANUP
        while(!clearStack.empty())
        {
            if(clearStack.top() == lastMarking) break;  // deleted elsewhere
            delete clearStack.top();
            clearStack.pop();
        }
#endif
    }


    void WorkflowSoundnessPTrie::setMetaParent(NonStrictMarking *marking) {
        PWListHybrid *pwhlist = dynamic_cast<PWListHybrid *>(this->pwList);

        if (marking->meta == nullptr) {
            marking->meta = new MetaDataWithTraceAndEncoding();
        }

        auto *mte =
                static_cast<MetaDataWithTraceAndEncoding *>(marking->meta);
        mte->parent = pwhlist->parent;
        mte->generatedBy = marking->getGeneratedBy();
    }


    WorkflowSoundness::ModelType WorkflowSoundness::calculateModelType() {
        bool isin, isout;
        bool hasInvariant = false;
        for (TimedPlace::Vector::const_iterator iter = tapn.getPlaces().begin();
             iter != tapn.getPlaces().end(); iter++) {
            isin = isout = true;
            TimedPlace *p = (*iter);
            if (p->getInputArcs().empty() && p->getOutputArcs().empty() && p->getTransportArcs().empty()) {
                bool continueOuter = true;
                // Test if really orphan place or if out place
                for (TransportArc::Vector::const_iterator trans_i = tapn.getTransportArcs().begin();
                     trans_i != tapn.getTransportArcs().end(); ++trans_i) {
                    if (&((*trans_i)->getDestination()) == p) {
                        continueOuter = false;
                        break;
                    }
                }
                if (continueOuter) continue; // Fix orphan places
            }

            if (!hasInvariant && p->getInvariant() != p->getInvariant().LS_INF) {
                hasInvariant = true;
            }

            if (p->getInputArcs().size() > 0) {
                isout = false;
            }

            if (p->getOutputArcs().size() > 0) {
                isin = false;
            }

            if (isout) {
                for (TransportArc::Vector::const_iterator iter = p->getTransportArcs().begin();
                     iter != p->getTransportArcs().end(); iter++) {
                    if (&(*iter)->getSource() == p) {
                        isout = false;
                        break;
                    }
                }
            }

            if (isin) {
                for (TransportArc::Vector::const_iterator iter = tapn.getTransportArcs().begin(); iter !=
                                                                                                  tapn.getTransportArcs().end(); ++iter) { // TODO maybe transportArcs should contain both incoming and outgoing? Might break something though.
                    if (&(*iter)->getDestination() == p) {
                        isin = false;
                        break;
                    }
                }
            }

            if (isin) {
                if (in == NULL) {
                    in = p;
                } else {
                    return NOTTAWFN;
                }
            }

            if (isout) {
                if (out == NULL) {
                    out = p;
                } else {
                    return NOTTAWFN;
                }
            }

        }

        if (in == nullptr || out == nullptr || in == out) {
            return NOTTAWFN;
        }

        if (initialMarking.size() != 1 || initialMarking.numberOfTokensInPlace(in->getIndex()) != 1) {
            return NOTTAWFN;
        }

        bool hasUrgent = false;
        bool hasInhibitor = false;
        // All transitions must have preset
        for (TimedTransition::Vector::const_iterator iter = tapn.getTransitions().begin();
             iter != tapn.getTransitions().end(); iter++) {
            if ((*iter)->getPresetSize() == 0 && (*iter)->getNumberOfTransportArcs() == 0) {
                return NOTTAWFN;
            }

            if (!hasUrgent && (*iter)->isUrgent()) {
                hasUrgent = true;
            }

            if (!hasInhibitor && !(*iter)->getInhibitorArcs().empty()) {
                hasInhibitor = true;
            }
        }

        if (hasUrgent || hasInvariant || hasInhibitor) {
            return ETAWFN;
        }

        return MTAWFN;
    }

    WorkflowSoundness::~WorkflowSoundness() {
        delete lastMarking;
        pwList->deleteWaitingList();
        delete pwList;
    }

} /* namespace VerifyTAPN */
