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
: tapn(tapn), initialMarking(initialMarking), query(query), options(options), successorGenerator( tapn, *this ){

}

WorkflowSoundness::WorkflowSoundness(TAPN::TimedArcPetriNet& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<NonStrictMarking>* waiting_list)
: pwList(new PWList(waiting_list, false)), tapn(tapn), initialMarking(initialMarking), query(query), options(options), successorGenerator( tapn, *this ), in(NULL), out(NULL), modelType(calculateModelType()){

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

	// TODO Phase 2


	return true;
}

bool WorkflowSoundness::addToPW(NonStrictMarking* marking, NonStrictMarking* parent){
	marking->cut();

	// TODO add to parents_set

	unsigned int size = marking->size();

	// Test if final place
	if(marking->numberOfTokensInPlace(out->getIndex()) > 0){
		if(size == marking->numberOfTokensInPlace(out->getIndex())){
			final_set->push_back(*marking);
		}else{
			return true;	// Terminate
		}
	}

	// If new marking
	if(pwList->add(marking)){

		// TODO Test if a covered marking has already reached

		// TODO Check k-bound?
		pwList->setMaxNumTokensIfGreater(size);
		if(size > options.getKBound()) {
			delete marking;
			return false;
		}
	}

	return false;
}

WorkflowSoundness::ModelType WorkflowSoundness::calculateModelType(){
	bool isin, isout;
	for(TimedPlace::Vector::const_iterator iter = tapn.getPlaces().begin(); iter != tapn.getPlaces().end(); iter++){
		isin = isout = true;
		TimedPlace* p = (*iter);

		if(p->getInputArcs().size() > 0){
			isout = false;
		}

		if(p->getOutputArcs().size() > 0){
			isin = false;
		}

		for(TransportArc::Vector::const_iterator iter = p->getTransportArcs().begin(); iter != p->getTransportArcs().end(); iter++){
			if(&(*iter)->getSource() == p){
				isout = false;
			}
			if(&(*iter)->getDestination() == p){
				isin = false;
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

	return ETAWFN;
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
	generateTraceStack(lastMarking, &printStack);
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
