/*
 * NonStrictSearch.cpp
 *
 *  Created on: 26/04/2012
 *      Author: MathiasGS
 */

#include "TimeDartLiveness.hpp"

namespace VerifyTAPN {
    namespace DiscreteVerification {

        bool TimeDartLiveness::verify() {
            if (addToPW(&initialMarking, NULL, INT_MAX)) {
                return true;
            }

            //Main loop
            while (pwList->hasWaitingStates()) {
                WaitingDart* waitingDart = pwList->getNextUnexplored();
                exploredMarkings++;
                
                // Add trace meta data ("add to trace")
                if (waitingDart->parent != NULL) {
                    if (((LivenessDart*)waitingDart->parent->dart)->traceData == NULL) {
                        TraceMetaDataList* list = new TraceMetaDataList();
                        ((LivenessDart*)waitingDart->parent->dart)->traceData = list;
                    }
                    ((LivenessDart*)waitingDart->parent->dart)->traceData->push_back(waitingDart);
                }

                // Detect ability to delay forever
                if (canDelayForever(waitingDart->dart->getBase())) {
                    NonStrictMarkingBase* lm = new NonStrictMarkingBase(*waitingDart->dart->getBase());
                    lm->setGeneratedBy(waitingDart->dart->getBase()->getGeneratedBy());
                    // lastMarking = new TraceList(lm, waitingDart.upper);
                    lastMarking = waitingDart;
                    return true;
                }

                // Initialize
                int maxCalculatedEnd = -1;

                // Set passed
                int passed = waitingDart->dart->getPassed();

                // Skip if already passed
                if (passed <= waitingDart->w) {
                    if (waitingDart->parent != NULL) {
                        ((LivenessDart*)waitingDart->parent->dart)->traceData->pop_back();
                    }
                    pwList->popWaiting();
                    continue;
                }

                waitingDart->dart->setPassed(waitingDart->w);
                this->tmpdart = waitingDart;
                // Iterate over transitions
                for (TimedTransition::Vector::const_iterator transition_iter = tapn->getTransitions().begin();
                        transition_iter != tapn->getTransitions().end(); transition_iter++) {
                    TimedTransition& transition = **transition_iter;

                    // Calculate enabled set
                    pair<int, int> calculatedStart = calculateStart(transition, waitingDart->dart->getBase());
                    if (calculatedStart.first == -1) { // Transition cannot be enabled in marking
                        continue;
                    }

                    // Aggregate on calculatedEnd for deadlock detection
                    if (calculatedStart.second > maxCalculatedEnd) {
                        maxCalculatedEnd = calculatedStart.second;
                    }

                    // Calculate start and end
                    int start = max(waitingDart->w, calculatedStart.first);
                    int end = min(passed - 1, calculatedStart.second);
                    if (start <= end) {
                        int stop = max(start, calculateStop(transition, waitingDart->dart->getBase()));
                        int finalStop = min(stop, end);
                        for (int n = start; n <= finalStop; n++) {
                            NonStrictMarkingBase Mpp(*waitingDart->dart->getBase());
                            Mpp.incrementAge(n);
                            int _end = n;
                            if (n == stop) {
                                _end = calculatedStart.second;
                            }

                            this->tmpupper = _end;

                            if(successorGenerator.generateAndInsertSuccessors(Mpp, transition)){
                                return true;
                            }
                            
                            pwList->flushBuffer();
                        }
                    }
                }

                // Detect deadlock
                if (maxCalculatedEnd < maxPossibleDelay(waitingDart->dart->getBase())) {
                    //lastMarking = new TraceList(waitingDart.dart->getBase(), maxPossibleDelay(waitingDart.dart->getBase()));
                    lastMarking = waitingDart;
                    deadlock = true;
                    return true; /* DEADLOCK! */
                }
                deleteBase(waitingDart->dart->getBase());
            }

            return false;
        }



        bool TimeDartLiveness::canDelayForever(NonStrictMarkingBase* marking) {
            for (PlaceList::const_iterator p_iter = marking->getPlaceList().begin(); p_iter != marking->getPlaceList().end(); p_iter++) {
                if (p_iter->place->GetInvariant().getBound() < INT_MAX) {
                    return false;
                }
            }
            return true;
        }

        bool TimeDartLiveness::addToPW(NonStrictMarkingBase* marking, WaitingDart* parent, int upper) {
            int start;
            if(options.getTrace() == VerificationOptions::SOME_TRACE){
                start = marking->getYoungest();
            }
            marking->cut();
            const TimedTransition* transition = marking->getGeneratedBy();
            unsigned int size = marking->size();

            pwList->setMaxNumTokensIfGreater(size);

            if (size > options.getKBound()) {
                delete marking;
                return false;
            }

            int youngest = marking->makeBase(tapn.get());

            QueryVisitor<NonStrictMarkingBase> checker(*marking);
            boost::any context;
            query->accept(checker, context);
            if (boost::any_cast<bool>(context)) {
                std::pair < LivenessDart*, bool> result = pwList->add(tapn.get(), marking, youngest, parent, upper, start);


                if (parent != NULL && parent->dart->getBase()->equals(*result.first->getBase()) && youngest <= upper) {
                    loop = true;
//                    lastMarking = parent;     // removed to fix loop-detection delay
                }

                //Find the dart created in the PWList
                if (result.first->traceData != NULL) {
                    for (TraceMetaDataList::const_iterator iter = result.first->traceData->begin(); iter != result.first->traceData->end(); iter++) {
                        if ((*iter)->parent->dart->getBase()->equals(*result.first->getBase()) && youngest <= (*iter)->upper) {
//                            lastMarking = (*iter); // removed to fix loop-detection delay
                            loop = true;
                            break;
                        }
                    }
                }

                if (loop) {
                    NonStrictMarkingBase* lm = new NonStrictMarkingBase(*result.first->getBase());
                    lm->setParent(parent->dart->getBase());
                    //lastMarking = new TraceList(lm, upper);   
                    if (options.getTrace()) {
//                        TraceDart* t = new TraceDart(*(TraceDart*) lastMarking);      // removed to fix loop-detection delay
                        lastMarking = new TraceDart(result.first, parent, result.first->getWaiting(), start, upper, transition);
//                        t->parent = lastMarking;      // removed to fix loop-detection delay
//                        lastMarking = t;              // removed to fix loop-detection delay

                    } else {
                        lastMarking = new WaitingDart(result.first, parent, result.first->getWaiting(), upper);
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
            std::cout << "  stored markings:\t" << pwList->size() << std::endl;
        }

        TimeDartLiveness::~TimeDartLiveness() {
        }

    } /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
