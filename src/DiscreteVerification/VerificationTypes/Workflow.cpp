/*
 * NonStrictSearch.cpp
 *
 *  Created on: 26/04/2012
 *      Author: MathiasGS
 */

#include "Workflow.hpp"

namespace VerifyTAPN {
    namespace DiscreteVerification {

        Workflow::Workflow(TAPN::TimedArcPetriNet& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<NonStrictMarking>* waiting_list) :
        tapn(tapn), initialMarking(initialMarking), query(query), options(options), in(NULL), out(NULL), modelType(calculateModelType()), pwList(new WorkflowPWList(waiting_list)), successorGenerator(tapn, *this) {
            for (TimedPlace::Vector::const_iterator iter = tapn.getPlaces().begin(); iter != tapn.getPlaces().end(); iter++) {
                if ((*iter)->getType() == Dead) {
                    (*iter)->setType(Std);
                }
            }
        }

        bool Workflow::isDelayPossible(NonStrictMarking& marking) {
            const PlaceList& places = marking.getPlaceList();
            if (places.size() == 0) return true; //Delay always possible in empty markings

            PlaceList::const_iterator markedPlace_iter = places.begin();
            for (TAPN::TimedPlace::Vector::const_iterator place_iter = tapn.getPlaces().begin(); place_iter != tapn.getPlaces().end(); place_iter++) {
                int inv = (*place_iter)->getInvariant().getBound();
                if (**place_iter == *(markedPlace_iter->place)) {
                    if (markedPlace_iter->maxTokenAge() > inv - 1) {
                        return false;
                    }

                    markedPlace_iter++;

                    if (markedPlace_iter == places.end()) return true;
                }
            }
            assert(false); // This happens if there are markings on places not in the TAPN
            return false;
        }

        Workflow::ModelType Workflow::calculateModelType() {
            bool isin, isout;
            bool hasInvariant = false;
            for (TimedPlace::Vector::const_iterator iter = tapn.getPlaces().begin(); iter != tapn.getPlaces().end(); iter++) {
                isin = isout = true;
                TimedPlace* p = (*iter);
                if (p->getInputArcs().empty() && p->getOutputArcs().empty() && p->getTransportArcs().empty()) {
                    bool continueOuter = true;
                    // Test if really orphan place or if out place
                    for (TransportArc::Vector::const_iterator trans_i = tapn.getTransportArcs().begin(); trans_i != tapn.getTransportArcs().end(); ++trans_i) {
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
                    for (TransportArc::Vector::const_iterator iter = p->getTransportArcs().begin(); iter != p->getTransportArcs().end(); iter++) {
                        if (&(*iter)->getSource() == p) {
                            isout = false;
                            break;
                        }
                    }
                }

                if (isin) {
                    for (TransportArc::Vector::const_iterator iter = tapn.getTransportArcs().begin(); iter != tapn.getTransportArcs().end(); ++iter) { // TODO maybe transportArcs should contain both incoming and outgoing? Might break something though.
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

            if (in == NULL || out == NULL || in == out) {
                return NOTTAWFN;
            }

            if (initialMarking.size() != 1 || initialMarking.numberOfTokensInPlace(in->getIndex()) != 1) {
                return NOTTAWFN;
            }

            bool hasUrgent = false;
            bool hasInhibitor = false;
            // All transitions must have preset
            for (TimedTransition::Vector::const_iterator iter = tapn.getTransitions().begin(); iter != tapn.getTransitions().end(); iter++) {
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

    } /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
