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
        : AbstractNaiveVerification<WorkflowPWList>(tapn, initialMarking, query, options, new WorkflowPWList(waiting_list)), in(NULL), out(NULL), modelType(calculateModelType()) {
            for (TimedPlace::Vector::const_iterator iter = tapn.getPlaces().begin(); iter != tapn.getPlaces().end(); iter++) {
                if ((*iter)->getType() == Dead) {
                    (*iter)->setType(Std);
                }
            }
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
