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
	: AbstractNaiveVerification<PWListBase,NonStrictMarking>(tapn, initialMarking, query, options, NULL){

}
    
ReachabilitySearch::ReachabilitySearch(TAPN::TimedArcPetriNet& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<NonStrictMarking*>* waiting_list)
	: AbstractNaiveVerification<PWListBase,NonStrictMarking>(tapn, initialMarking, query, options, new PWList(waiting_list, false)) {
    
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
                if(res == ADDTOPW_RETURNED_TRUE){
                    return true;
                }  else if (res == ADDTOPW_RETURNED_FALSE_URGENTENABLED) {
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

bool ReachabilitySearch::addToPW(NonStrictMarking* marking, NonStrictMarking* parent){
	marking->cut(placeStats);
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
}

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
