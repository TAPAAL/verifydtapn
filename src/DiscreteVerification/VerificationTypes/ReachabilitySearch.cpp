/*
 * NonStrictSearch.cpp
 *
 *  Created on: 26/04/2012
 *      Author: MathiasGS
 */

#include "ReachabilitySearch.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

ReachabilitySearch::ReachabilitySearch(boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options)
	: tapn(tapn), initialMarking(initialMarking), query(query), options(options), successorGenerator( *tapn.get(), *this ){

}
    
ReachabilitySearch::ReachabilitySearch(boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<NonStrictMarking>* waiting_list)
	: pwList(new PWList(waiting_list, false)), tapn(tapn), initialMarking(initialMarking), query(query), options(options), successorGenerator( *tapn.get(), *this ){

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

		// Generate next markings
		if(isDelayPossible(next_marking)){
			NonStrictMarking* marking = new NonStrictMarking(next_marking);
			marking->incrementAge();
			marking->setGeneratedBy(NULL);
                        if(addToPW(marking, &next_marking)){
                            return true;
                        }
		}
                if(successorGenerator.generateAndInsertSuccessors(next_marking)){
                    return true;
                }  

                deleteMarking(&next_marking);
	}

	return false;
}

bool ReachabilitySearch::isDelayPossible(NonStrictMarking& marking){
	const PlaceList& places = marking.getPlaceList();
	if(places.size() == 0) return true;	//Delay always possible in empty markings

	PlaceList::const_iterator markedPlace_iter = places.begin();
	for(TAPN::TimedPlace::Vector::const_iterator place_iter = tapn->getPlaces().begin(); place_iter != tapn->getPlaces().end(); place_iter++){
		int inv = place_iter->get()->GetInvariant().getBound();
		if(*(place_iter->get()) == *(markedPlace_iter->place)){
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
		QueryVisitor<NonStrictMarking> checker(*marking);
		boost::any context;
		query->accept(checker, context);
		if(boost::any_cast<bool>(context)) {
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
		printXMLTrace(lastMarking, printStack, query->getQuantifier());
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
        printXMLTrace(lastMarking, printStack, query->getQuantifier());
}

ReachabilitySearch::~ReachabilitySearch() {
	// TODO Auto-generated destructor stub
}

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
