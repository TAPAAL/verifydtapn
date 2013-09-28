/*
 * NonStrictSearch.cpp
 *
 *  Created on: 26/04/2012
 *      Author: MathiasGS
 */

#include "WorkflowSoundness.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

WorkflowSoundness::WorkflowSoundness(TAPN::TimedArcPetriNet& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options)
: tapn(tapn), initialMarking(initialMarking), query(query), options(options), successorGenerator( tapn, *this ), final_set(new vector<NonStrictMarking*>), min_exec(INT_MAX){

}

WorkflowSoundness::WorkflowSoundness(TAPN::TimedArcPetriNet& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<NonStrictMarking>* waiting_list)
: pwList(new WorkflowPWList(waiting_list)), tapn(tapn), initialMarking(initialMarking), query(query), options(options), successorGenerator( tapn, *this ), in(NULL), out(NULL), modelType(calculateModelType()), final_set(new vector<NonStrictMarking*>), lastMarking(NULL){

	for(TimedPlace::Vector::const_iterator iter = tapn.getPlaces().begin(); iter != tapn.getPlaces().end(); iter++){
		if((*iter)->getType() == Dead){
			(*iter)->setType(Std);
		}
	}
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
		return true;	// Terminate false, TODO: throw error message?
	}

	// Map to existing marking if any
	NonStrictMarking* lookup = pwList->lookup(marking);
	if(lookup != NULL){
		marking = lookup;
	}else{
		marking->meta = new MetaData();
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
		((WorkflowSoundnessMetaData*)marking->meta)->min = 0;		// Initial markings
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

WorkflowSoundness::ModelType WorkflowSoundness::calculateModelType(){
	bool isin, isout;
	bool hasInvariant = false;
	for(TimedPlace::Vector::const_iterator iter = tapn.getPlaces().begin(); iter != tapn.getPlaces().end(); iter++){
		isin = isout = true;
		TimedPlace* p = (*iter);
		if(p->getInputArcs().empty() && p->getOutputArcs().empty() && p->getTransportArcs().empty())	continue;	// Fix unused places

		if(!hasInvariant && p->getInvariant() != p->getInvariant().LS_INF){
			hasInvariant = true;
		}

		if(p->getInputArcs().size() > 0){
			isout = false;
		}

		if(p->getOutputArcs().size() > 0){
			isin = false;
		}

		if(isout){
			for(TransportArc::Vector::const_iterator iter = p->getTransportArcs().begin(); iter != p->getTransportArcs().end(); iter++){
				if(&(*iter)->getSource() == p){
					isout = false;
					break;
				}
			}
		}

		if(isin){
			for(TransportArc::Vector::const_iterator iter = tapn.getTransportArcs().begin(); iter != tapn.getTransportArcs().end(); ++iter){		// TODO maybe transportArcs should contain both incoming and outgoing? Might break something though.
				if(&(*iter)->getDestination() == p){
					isin = false;
					break;
				}
			}
		}

		if(isin){
			if(in == NULL){
				in = p;
			}else{
				return NOTTAWFN;
			}
		}

		if(isout){
			if(out == NULL){
				out = p;
			}else{
				return NOTTAWFN;
			}
		}

	}

	if(in == NULL || out == NULL || in == out){
		return NOTTAWFN;
	}

	if(initialMarking.size() != 1 || initialMarking.numberOfTokensInPlace(in->getIndex()) != 1){
		return NOTTAWFN;
	}

	bool hasUrgent = false;
	bool hasInhibitor = false;
	// All transitions must have preset
	for(TimedTransition::Vector::const_iterator iter = tapn.getTransitions().begin(); iter != tapn.getTransitions().end(); iter++){
		if((*iter)->getPresetSize() == 0 && (*iter)->getNumberOfTransportArcs() == 0){
			return NOTTAWFN;
		}

		if(!hasUrgent && (*iter)->isUrgent()){
			hasUrgent = true;
		}

		if(!hasInhibitor && !(*iter)->getInhibitorArcs().empty()){
			hasInhibitor = true;
		}
	}

	if(hasUrgent || hasInvariant || hasInhibitor){
		return ETAWFN;
	}

	return MTAWFN;
}

bool WorkflowSoundness::isDelayPossible(NonStrictMarking& marking){
	const PlaceList& places = marking.getPlaceList();
	if(places.size() == 0) return true;	//Delay always possible in empty markings

	PlaceList::const_iterator markedPlace_iter = places.begin();
	for(TAPN::TimedPlace::Vector::const_iterator place_iter = tapn.getPlaces().begin(); place_iter != tapn.getPlaces().end(); place_iter++){
		int inv = (*place_iter)->getInvariant().getBound();
		if(**place_iter == *(markedPlace_iter->place)){
			if(markedPlace_iter->maxTokenAge() > inv-1){
				return false;
			}

			markedPlace_iter++;

			if(markedPlace_iter == places.end())	return true;
		}
	}
	assert(false);	// This happens if there are markings on places not in the TAPN
	return false;
}

void WorkflowSoundness::printStats(){
	std::cout << "  discovered markings:\t" << pwList->discoveredMarkings << std::endl;
	std::cout << "  explored markings:\t" << pwList->size()-pwList->explored() << std::endl;
	std::cout << "  stored markings:\t" << pwList->size() << std::endl;
}

void WorkflowSoundness::getTrace(){
	stack < NonStrictMarking*> printStack;
	NonStrictMarking* next = lastMarking;
	while(((WorkflowSoundnessMetaData*)next->meta)->parents != NULL && !((WorkflowSoundnessMetaData*)next->meta)->parents->empty()){
		int min = INT_MAX;
		NonStrictMarking* parent = NULL;
		for(vector<NonStrictMarking*>::const_iterator iter = ((WorkflowSoundnessMetaData*)next->meta)->parents->begin(); iter != ((WorkflowSoundnessMetaData*)next->meta)->parents->end(); ++iter){
			if(((WorkflowSoundnessMetaData*)(*iter)->meta)->min < min){
				min = ((WorkflowSoundnessMetaData*)(*iter)->meta)->min;
				parent = *iter;
			}
		}

		printStack.push(next);
		if(((WorkflowSoundnessMetaData*)next->meta)->inTrace){ printStack.push(next); break;	}
		((WorkflowSoundnessMetaData*)next->meta)->inTrace = true;
		next = parent;
	}

	printStack.push(next);

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
