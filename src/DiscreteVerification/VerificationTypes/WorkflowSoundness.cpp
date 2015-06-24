/*
 * NonStrictSearch.cpp
 *
 *  Created on: 26/04/2012
 *      Author: MathiasGS
 */

#include "WorkflowSoundness.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

WorkflowSoundness::WorkflowSoundness(TAPN::TimedArcPetriNet& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<NonStrictMarking*>* waiting_list)
: Workflow(tapn, initialMarking, query, options), passedStack(), minExec(INT_MAX), linearSweepTreshold(3), coveredMarking(NULL), modelType(calculateModelType()){
    pwList = new WorkflowPWList(waiting_list);
}


WorkflowSoundness::WorkflowSoundness(TAPN::TimedArcPetriNet& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options)
: Workflow(tapn, initialMarking, query, options), passedStack(), minExec(INT_MAX), linearSweepTreshold(3), coveredMarking(NULL), modelType(calculateModelType()){
    
};


bool WorkflowSoundness::verify(){
	if(addToPW(&initialMarking, NULL)){
		return false;
	}

	// Phase 1
	while(pwList->hasWaitingStates()){

		NonStrictMarking* next_marking = pwList->getNextUnexplored();
		tmpParent = next_marking;
		bool noDelay = false;

		Result res = successorGenerator.generateAndInsertSuccessors(*next_marking);
		if(res == ADDTOPW_RETURNED_TRUE){
			return false;
		}  else if (res == ADDTOPW_RETURNED_FALSE_URGENTENABLED) {
			noDelay = true;
		}

		// Generate next markings
		if(!noDelay && isDelayPossible(*next_marking)){
			NonStrictMarking* marking = new NonStrictMarking(*next_marking);
			marking->incrementAge();
			marking->setGeneratedBy(NULL);
			if(addToPW(marking, next_marking)){
				return false;
			}
		}
	}

	// Phase 2
        // mark all as passed which ends in a final marking        
        int passed = numberOfPassed();
        
        if(passed < pwList->stored){
                lastMarking = pwList->getUnpassed();
                return false;
        } else {
		return true;
	}
}

int WorkflowSoundness::numberOfPassed()
{
    int passed = 0;
    while(passedStack.size()){
        WorkflowSoundnessMetaData* next = 
                    static_cast<WorkflowSoundnessMetaData*>(passedStack.top());
        passedStack.pop();
        if(next->passed) continue;
        next->passed = true;
        ++passed;
        for(vector<MetaData*>::iterator iter = next->parents.begin();
                iter != next->parents.end(); iter++){
                passedStack.push(*iter);
        }
    }
    return passed;
}


bool WorkflowSoundness::addToPW(NonStrictMarking* marking, NonStrictMarking* parent){
	marking->cut(placeStats);

	unsigned int size = marking->size();

	// Check K-bound
	pwList->setMaxNumTokensIfGreater(size);
	if(modelType == ETAWFN && size > options.getKBound()) {
		lastMarking = marking;
		return true;	// Terminate false
	}

	// Map to existing marking if any
        bool isNew = false;
        marking->setParent(parent);
        NonStrictMarking* old = pwList->addToPassed(marking, false);
	if(old == NULL){
                isNew = true;
		marking->meta = new WorkflowSoundnessMetaData();
                marking->setParent(parent);
	} else  {
            delete marking;
            marking = old;
        }
 
	// add to parents_set
	if(parent != NULL){
                addParentMeta(marking->meta, parent->meta);
		if(marking->getGeneratedBy() == NULL){
			marking->meta->totalDelay = min(marking->meta->totalDelay, parent->meta->totalDelay+1);	// Delay
		}else{
			marking->meta->totalDelay = min(marking->meta->totalDelay, parent->meta->totalDelay);	// Transition
		}
	}else{
		marking->meta->totalDelay = 0;
	}



	// Test if final place
	if(marking->numberOfTokensInPlace(out->getIndex()) > 0){
		if(size == 1){
                        passedStack.push(marking->meta);
			// Set min
			marking->meta->totalDelay = min(marking->meta->totalDelay, parent->meta->totalDelay);	// Transition
                        // keep track of shortest trace
                        if (marking->meta->totalDelay < minExec) {
                            minExec = marking->meta->totalDelay;
                            lastMarking = marking;
                            return false;
                        }
		}else{
			lastMarking = marking;
			return true;	// Terminate false
		}
	}else{
		// If new marking
		if(isNew){
                        pwList->addLastToWaiting();
			if(parent != NULL && marking->canDeadlock(tapn, 0)){
				lastMarking = marking;
				return true;
			}
			if(modelType == MTAWFN && checkForCoveredMarking(marking)){
				lastMarking = marking;
				return true;	// Terminate false
			}
		}
	}
	return false;
}

void WorkflowSoundness::addParentMeta(MetaData* meta, MetaData* parent)
{
    WorkflowSoundnessMetaData* markingmeta = ((WorkflowSoundnessMetaData*)meta);
    markingmeta->parents.push_back(parent);

}


bool WorkflowSoundness::checkForCoveredMarking(NonStrictMarking* marking){
	if(marking->size() <= options.getKBound()){
		return false;	// Do not run check on small markings (invoke more rarely)
	}

	NonStrictMarking* covered = pwList->getCoveredMarking(marking, (marking->size() > linearSweepTreshold));
	if(covered != NULL){
            coveredMarking = covered;
            return true;
	}

	return false;
}

void WorkflowSoundness::getTrace(NonStrictMarking* marking){

	stack < NonStrictMarking*> printStack;
        NonStrictMarking* next = marking;
        if(next != NULL){
            do{
                printStack.push(next);
                next = (NonStrictMarking*)next->getParent();
            } while(next);
        }
        
        printXMLTrace(marking, printStack, query, tapn);

}



WorkflowSoundness::ModelType WorkflowSoundness::calculateModelType() {
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

WorkflowSoundness::~WorkflowSoundness() {
    delete lastMarking;
    pwList->deleteWaitingList();
    delete pwList;
}

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
