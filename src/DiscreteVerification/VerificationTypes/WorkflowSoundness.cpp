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
: Workflow(tapn, initialMarking, query, options, waiting_list), final_set(new vector<NonStrictMarking*>), min_exec(INT_MAX), linearSweepTreshold(3), coveredMarking(NULL){
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
		delete marking;
		marking = lookup;
	}else{
		marking->meta = new WorkflowSoundnessMetaData();
	}

	WorkflowSoundnessMetaData* marking_meta_data = ((WorkflowSoundnessMetaData*)marking->meta);

	// add to parents_set
	if(parent != NULL){
		WorkflowSoundnessMetaData* parent_meta_data = ((WorkflowSoundnessMetaData*)parent->meta);
		marking_meta_data->parents->push_back(parent);
		if(marking->getGeneratedBy() == NULL){
			marking_meta_data->min = min(marking_meta_data->min, parent_meta_data->min+1);	// Delay
		}else{
			marking_meta_data->min = min(marking_meta_data->min, parent_meta_data->min);	// Transition
		}
	}else{
		marking_meta_data->min = 0;
	}



	// Test if final place
	if(marking->numberOfTokensInPlace(out->getIndex()) > 0){
		if(size == 1){
			marking = pwList->addToPassed(marking);
			marking_meta_data = ((WorkflowSoundnessMetaData*)marking->meta);
			marking_meta_data->parents->push_back(parent);
			// Set min
			marking_meta_data->min = min(marking_meta_data->min, ((WorkflowSoundnessMetaData*)parent->meta)->min);	// Transition
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

void WorkflowSoundness::getTrace(NonStrictMarking* base){
	stack < NonStrictMarking*> printStack;
	NonStrictMarking* next = base;
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

	stack < NonStrictMarking*> tempStack;
	while(!printStack.empty()){
		printStack.top()->meta->inTrace = false;
		tempStack.push(printStack.top());
		printStack.pop();
	}

	while(!tempStack.empty()){
		printStack.push(tempStack.top());
		tempStack.pop();
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
