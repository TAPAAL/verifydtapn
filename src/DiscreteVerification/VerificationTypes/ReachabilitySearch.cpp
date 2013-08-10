/*
 * NonStrictSearch.cpp
 *
 *  Created on: 26/04/2012
 *      Author: MathiasGS
 */

#include "ReachabilitySearch.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

ReachabilitySearch::ReachabilitySearch(TAPN::TimedArcPetriNet& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options)
	: tapn(tapn), initialMarking(initialMarking), query(query), options(options), successorGenerator( tapn, *this ){

}
    
ReachabilitySearch::ReachabilitySearch(TAPN::TimedArcPetriNet& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<NonStrictMarking>* waiting_list)
	: pwList(new PWList(waiting_list, false)), tapn(tapn), initialMarking(initialMarking), query(query), options(options), successorGenerator( tapn, *this ){

}

bool ReachabilitySearch::verify(){
	if(addToPW(&initialMarking, NULL)){
		return true;
	}

	//Main loop
	while(pwList->hasWaitingStates()){
		NonStrictMarking& next_marking = *pwList->getNextUnexplored();
                tmpParent = &next_marking;
		trace.push(&next_marking);
		validChildren = 0;

                bool noDelay = false;
                Result res = successorGenerator.generateAndInsertSuccessors(next_marking);
                if(res == QUERY_SATISFIED){
                    return true;
                }  else if (res == URGENT_ENABLED) {
                    noDelay = true;
                }

		// Generate next markings
		if(!noDelay && isDelayPossible(next_marking)){
			NonStrictMarking* marking = new NonStrictMarking(next_marking);
			marking->incrementAge();
			marking->setGeneratedBy(NULL);
                        if(addToPW(marking, &next_marking)){
                            return true;
                        }
		}
                deleteMarking(&next_marking);

	}

	return false;
}

bool ReachabilitySearch::isDelayPossible(NonStrictMarking& marking){
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

bool ReachabilitySearch::addToPW(NonStrictMarking* marking, NonStrictMarking* parent){
	marking->cut();
	marking->setParent(parent);

	unsigned int size = marking->size();

	pwList->setMaxNumTokensIfGreater(size);

	if(size > options.getKBound()) {
		delete marking;
		return false;
	}

	if(pwList->add(marking)){
		QueryVisitor<NonStrictMarking> checker(*marking, tapn);
		BoolResult context;
		query->accept(checker, context);
		if(context.value) {
			lastMarking = marking;
			return true;
		} else {
                        deleteMarking(marking);
			return false;
		}
	} else {
		delete marking;
	}

	return false;
}

void ReachabilitySearch::printStats(){
	std::cout << "  discovered markings:\t" << pwList->discoveredMarkings << std::endl;
	std::cout << "  explored markings:\t" << pwList->size()-pwList->explored() << std::endl;
	std::cout << "  stored markings:\t" << pwList->size() << std::endl;
}

void ReachabilitySearch::getTrace(){
	stack < NonStrictMarking*> printStack;
	generateTraceStack(lastMarking, &printStack);
	if(options.getXmlTrace()){
		printXMLTrace(lastMarking, printStack, query, tapn);
	} else {
		printHumanTrace(lastMarking, printStack, query->getQuantifier());
	}
}

void ReachabilitySearchPTrie::getTrace(){
	stack < NonStrictMarking*> printStack;
        PWListHybrid* pwhlist = (PWListHybrid*)(this->pwList);
        MetaDataWithTraceAndEncoding* next = pwhlist->parent;
        NonStrictMarking* last = lastMarking;
        printStack.push(lastMarking);
        while(next != NULL){
            NonStrictMarking* m = pwhlist->decode(next->ep);
            m->setGeneratedBy(next->generatedBy);
            last->setParent(m);
            last = m;
            printStack.push(m);
            next = next->parent;
        };
        printXMLTrace(lastMarking, printStack, query, tapn);
}

ReachabilitySearch::~ReachabilitySearch() {
    delete pwList;
}

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
