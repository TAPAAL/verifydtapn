/*
 * NonStrictSearch.cpp
 *
 *  Created on: 26/04/2012
 *      Author: MathiasGS
 */

#include "WorkflowSoundness.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

WorkflowSoundness::WorkflowSoundness(TAPN::TimedArcPetriNet& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<NonStrictMarking>* waiting_list)
: Workflow(tapn, initialMarking, query, options, waiting_list), pwList(new WorkflowPWList(waiting_list)), final_set(new vector<NonStrictMarking*>), min_exec(INT_MAX), linearSweepTreshold(3){
}

bool WorkflowSoundness::verify(){
	if(addToPW(&initialMarking, NULL)){
		return false;
	}

	// Phase 1
	while(pwList->hasWaitingStates()){
		NonStrictMarking& next_marking = *pwList->getNextUnexplored();
		tmpParent = &next_marking;
		bool noDelay = false;
		Result res = successorGenerator.generateAndInsertSuccessors(next_marking);
		if(res == QUERY_SATISFIED){
			return false;
		}  else if (res == URGENT_ENABLED) {
			noDelay = true;
		}

		// Generate next markings
		if(!noDelay && isDelayPossible(next_marking)){
			NonStrictMarking* marking = new NonStrictMarking(next_marking);
			marking->incrementAge();
			marking->setGeneratedBy(NULL);
			if(addToPW(marking, &next_marking)){
				return false;
			}
		}
	}

	// Phase 2
	for(vector<NonStrictMarking*>::iterator iter = final_set->begin(); iter != final_set->end(); iter++){
		pwList->addToWaiting(*iter);
		if(((WorkflowSoundnessMetaData*)(*iter)->meta)->min < min_exec){
			min_exec = ((WorkflowSoundnessMetaData*)(*iter)->meta)->min;
			lastMarking = (*iter);
		}
	}

	while(pwList->hasWaitingStates()){
		NonStrictMarking* next_marking = pwList->getNextUnexplored();
		if(next_marking->meta->passed)	continue;
		next_marking->meta->passed = true;
		for(vector<NonStrictMarking*>::iterator iter = ((WorkflowSoundnessMetaData*)next_marking->meta)->parents->begin(); iter != ((WorkflowSoundnessMetaData*)next_marking->meta)->parents->end(); iter++){
			pwList->addToWaiting(*iter);
		}
	}

	NonStrictMarking* unpassed = pwList->getUnpassed();
	if(unpassed == NULL){
		return true;
	}else{
		lastMarking = unpassed;
		return false;
	}
}

bool WorkflowSoundness::addToPW(NonStrictMarking* marking, NonStrictMarking* parent){
	marking->cut();

	unsigned int size = marking->size();

	// Check K-bound
	pwList->setMaxNumTokensIfGreater(size);
	if(modelType == ETAWFN && size > options.getKBound()) {
		lastMarking = marking;
		return true;	// Terminate false
	}

	// Map to existing marking if any
	NonStrictMarking* lookup = pwList->lookup(marking);
	if(lookup != NULL){
		marking = lookup;
	}else{
		marking->meta = new WorkflowSoundnessMetaData();
	}

	// add to parents_set
	if(parent != NULL){
		((WorkflowSoundnessMetaData*)marking->meta)->parents->push_back(parent);
		if(marking->getGeneratedBy() == NULL){
			((WorkflowSoundnessMetaData*)marking->meta)->min = min(((WorkflowSoundnessMetaData*)marking->meta)->min, ((WorkflowSoundnessMetaData*)parent->meta)->min+1);	// Delay
		}else{
			((WorkflowSoundnessMetaData*)marking->meta)->min = min(((WorkflowSoundnessMetaData*)marking->meta)->min, ((WorkflowSoundnessMetaData*)parent->meta)->min);	// Transition
		}
	}else{
		((WorkflowSoundnessMetaData*)marking->meta)->min = 0;
	}



	// Test if final place
	if(marking->numberOfTokensInPlace(out->getIndex()) > 0){
		if((int) size == marking->numberOfTokensInPlace(out->getIndex())){
			marking = pwList->addToPassed(marking);
			((WorkflowSoundnessMetaData*)marking->meta)->parents->push_back(parent);
			// Set min
			((WorkflowSoundnessMetaData*)marking->meta)->min = min(((WorkflowSoundnessMetaData*)marking->meta)->min, ((WorkflowSoundnessMetaData*)parent->meta)->min);	// Transition
			final_set->push_back(marking);
		}else{
			lastMarking = marking;
			return true;	// Terminate false
		}
	}else{
		// If new marking
		if(pwList->add(marking)){
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

bool WorkflowSoundness::checkForCoveredMarking(NonStrictMarking* marking){
	vector<NonStrictMarking*> coveredMarkings;
	coveredMarkings.push_back(new NonStrictMarking(*marking));

	if(marking->size() > linearSweepTreshold){
		return pwList->isCoveredBy(marking);
	}

	for(PlaceList::const_iterator p_iter = marking->getPlaceList().begin(); p_iter != marking->getPlaceList().end(); ++p_iter){
		for(TokenList::const_iterator t_iter = p_iter->tokens.begin(); t_iter != p_iter->tokens.end(); ++t_iter){
			for(int i = 1; i <= t_iter->getCount(); ++i){
				vector<NonStrictMarking*> toAdd;
				for(vector<NonStrictMarking*>::iterator iter = coveredMarkings.begin(); iter != coveredMarkings.end(); ++iter){
					NonStrictMarking* new_marking = new NonStrictMarking(**iter);
					for(int ii = i; ii > 0; --ii){
						new_marking->removeToken(p_iter->place->getIndex(), t_iter->getAge());
					}
					toAdd.push_back(new_marking);
				}
				for(vector<NonStrictMarking*>::iterator iter = toAdd.begin(); iter != toAdd.end(); ++iter){
					coveredMarkings.push_back(*iter);
				}
			}
		}
	}

	bool isFirst = true;
	for(vector<NonStrictMarking*>::iterator iter = coveredMarkings.begin(); iter != coveredMarkings.end(); ++iter){
		if(isFirst){
			isFirst = false;
			continue;
		}
		if(pwList->lookup(*iter) != NULL){
			return true;
		}
		delete *iter;
	}

	return false;
}

void WorkflowSoundness::getTrace(){
	stack < NonStrictMarking*> printStack;
	NonStrictMarking* next = lastMarking;
	do{
		int min = INT_MAX;
		NonStrictMarking* parent = NULL;
		for(vector<NonStrictMarking*>::const_iterator iter = ((WorkflowSoundnessMetaData*)next->meta)->parents->begin(); iter != ((WorkflowSoundnessMetaData*)next->meta)->parents->end(); ++iter){
			if(((WorkflowSoundnessMetaData*)(*iter)->meta)->min < min){
				min = ((WorkflowSoundnessMetaData*)(*iter)->meta)->min;
				parent = *iter;
			}
		}

		if(((WorkflowSoundnessMetaData*)next->meta)->inTrace){ break;	}
		((WorkflowSoundnessMetaData*)next->meta)->inTrace = true;
		printStack.push(next);
		next = parent;

	}while(((WorkflowSoundnessMetaData*)next->meta)->parents != NULL && !((WorkflowSoundnessMetaData*)next->meta)->parents->empty());

	if(printStack.top() != next){
		printStack.push(next);
	}

	if(options.getXmlTrace()){
		printXMLTrace(lastMarking, printStack, query, tapn);
	} else {
		printHumanTrace(lastMarking, printStack, query->getQuantifier());
	}
}

WorkflowSoundness::~WorkflowSoundness() {
}

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
