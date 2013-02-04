/*
 * NonStrictSearch.cpp
 *
 *  Created on: 26/04/2012
 *      Author: MathiasGS
 */

#include "TimeDartLiveness.hpp"

namespace VerifyTAPN {
    namespace DiscreteVerification {

        bool TimeDartLiveness::Verify() {
            if (addToPW(&initialMarking, NULL, INT_MAX)) {
                return true;
            }

            //Main loop
            while (pwList->HasWaitingStates()) {
                WaitingDart& waitingDart = *pwList->GetNextUnexplored();
                exploredMarkings++;

                // Add trace meta data ("add to trace")
                if (waitingDart.parent != NULL) {
                    if (waitingDart.parent->dart->traceData == NULL) {
                        TraceMetaDataList* list = new TraceMetaDataList();
                        waitingDart.parent->dart->traceData = list;
                    }
                    waitingDart.parent->dart->traceData->push_back(&waitingDart);
                }

                // Detect ability to delay forever
                if (canDelayForever(waitingDart.dart->getBase())) {
                    NonStrictMarkingBase* lm = new NonStrictMarkingBase(*waitingDart.dart->getBase());
                    lm->generatedBy = waitingDart.dart->getBase()->generatedBy;
                   // lastMarking = new TraceList(lm, waitingDart.upper);
                    lastMarking = &waitingDart;
                    return true;
                }

                // Initialize
                int maxCalculatedEnd = -1;

                // Set passed
                int passed = waitingDart.dart->getPassed();

                // Skip if already passed
                if (passed <= waitingDart.w) {
                    if (waitingDart.parent != NULL) {
                        waitingDart.parent->dart->traceData->pop_back();
                    }
                    pwList->PopWaiting();
                    continue;
                }

                waitingDart.dart->setPassed(waitingDart.w);

                // Iterate over transitions
                for (TimedTransition::Vector::const_iterator transition_iter = tapn->GetTransitions().begin();
                        transition_iter != tapn->GetTransitions().end(); transition_iter++) {
                    TimedTransition& transition = **transition_iter;

                    // Calculate enabled set
                    pair<int, int> calculatedStart = calculateStart(transition, waitingDart.dart->getBase());
                    if (calculatedStart.first == -1) { // Transition cannot be enabled in marking
                        continue;
                    }

                    // Aggregate on calculatedEnd for deadlock detection
                    if (calculatedStart.second > maxCalculatedEnd) {
                        maxCalculatedEnd = calculatedStart.second;
                    }

                    // Calculate start and end
                    int start = max(waitingDart.w, calculatedStart.first);
                    int end = min(passed - 1, calculatedStart.second);
                    if (start <= end) {
                        int stop = max(start, calculateStop(transition, waitingDart.dart->getBase()));
                        int finalStop = min(stop, end);
                        for (int n = start; n <= finalStop; n++) {
                            NonStrictMarkingBase Mpp(*waitingDart.dart->getBase());
                            Mpp.incrementAge(n);
                            int _end = n;
                            if (n == stop) {
                                _end = calculatedStart.second;
                            }

                            vector<NonStrictMarkingBase*> next = getPossibleNextMarkings(Mpp, transition);
                            for (vector<NonStrictMarkingBase*>::iterator it = next.begin(); it != next.end(); it++) {

                                if (addToPW(*it, &waitingDart, _end)) {
                                     return true;
                                }
                            }

                            pwList->flushBuffer();
                        }
                    }
                }

                // Detect deadlock
                if (maxCalculatedEnd < maxPossibleDelay(waitingDart.dart->getBase())) {
                    //lastMarking = new TraceList(waitingDart.dart->getBase(), maxPossibleDelay(waitingDart.dart->getBase()));
                    lastMarking = &waitingDart;
                    return true; /* DEADLOCK! */
                }
                deleteBase(waitingDart.dart->getBase());
            }

            return false;
        }

        void TimeDartLiveness::GetTrace() {
            stack<NonStrictMarkingBase*> traceStack;
            int upper = lastMarking->w;
            TraceDart* trace = lastMarking;

            while(trace != NULL){
                NonStrictMarkingBase* m = new NonStrictMarkingBase(*(trace->dart->getBase()));
                m->SetGeneratedBy(trace->generatedBy);
                m->incrementAge(upper);
                traceStack.push(m);
                upper = trace->upper;
                trace = trace->parent;
            }

            
            traceStack.push(&initialMarking);
            
            PrintXMLTrace(lastMarking->dart->getBase(), traceStack, query->GetQuantifier());
        }

        bool TimeDartLiveness::canDelayForever(NonStrictMarkingBase* marking) {
            for (PlaceList::const_iterator p_iter = marking->GetPlaceList().begin(); p_iter != marking->GetPlaceList().end(); p_iter++) {
                if (p_iter->place->GetInvariant().GetBound() < INT_MAX) {
                    return false;
                }
            }
            return true;
        }

        bool TimeDartLiveness::addToPW(NonStrictMarkingBase* marking, WaitingDart* parent, int upper) {
            marking->cut();
            TimedTransition* transition = marking->generatedBy;
            unsigned int size = marking->size();

            pwList->SetMaxNumTokensIfGreater(size);

            if (size > options.GetKBound()) {
                delete marking;
                return false;
            }

            int youngest = marking->makeBase(tapn.get());

            QueryVisitor<NonStrictMarkingBase> checker(*marking);
            boost::any context;
            query->Accept(checker, context);
            if (boost::any_cast<bool>(context)) {
                std::pair < TimeDart*, bool> result = pwList->Add(tapn.get(), marking, youngest, parent, upper);


                if (parent != NULL && parent->dart->getBase()->equals(*result.first->getBase()) && youngest <= upper) {
                    loop = true;
                    lastMarking = parent;
                }

                //Find the dart created in the PWList
                if (result.first->traceData != NULL) {
                    for (TraceMetaDataList::const_iterator iter = result.first->traceData->begin(); iter != result.first->traceData->end(); iter++) {
                        if ((*iter)->parent->dart->getBase()->equals(*result.first->getBase()) && youngest <= (*iter)->upper) {
                            lastMarking = (*iter);
                            loop = true;
                            break;
                        }
                    }
                }

                if (loop) {
                    NonStrictMarkingBase* lm = new NonStrictMarkingBase(*result.first->getBase());
                    lm->parent = parent->dart->getBase();
                    //lastMarking = new TraceList(lm, upper);
                    if(options.GetTrace()){
                        TraceDart* t = new TraceDart(*(TraceDart*)lastMarking);
                        lastMarking = new TraceDart(result.first, parent, result.first->getWaiting(), upper, transition ); 
                        t->parent = lastMarking;
                        lastMarking = t;

                    } else {
                        lastMarking = new WaitingDart(result.first, parent, result.first->getWaiting(), upper );
                    }
                    return true;
                }
            }
            deleteBase(marking);
            return false;
        }

        void TimeDartLiveness::printStats() {
            std::cout << "  discovered markings:\t" << pwList->discoveredMarkings << std::endl;
            std::cout << "  explored markings:\t" << exploredMarkings << std::endl;
            std::cout << "  stored markings:\t" << pwList->Size() << std::endl;
        }

        TimeDartLiveness::~TimeDartLiveness() {
        }

    } /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
