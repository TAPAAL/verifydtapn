/*
 * NonStrictSearch.cpp
 *
 *  Created on: 26/04/2012
 *      Author: MathiasGS
 */

#include "LivenessSearch.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

LivenessSearch::LivenessSearch(boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options)
	: tapn(tapn), initialMarking(initialMarking), query(query), options(options), successorGenerator( *tapn.get(), *this ){

}
    
LivenessSearch::LivenessSearch(boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<NonStrictMarking>* waiting_list)
	: pwList(new PWList(waiting_list, true)), tapn(tapn), initialMarking(initialMarking), query(query), options(options), successorGenerator( *tapn.get(), *this ){

}

bool LivenessSearch::verify(){
	if(addToPW(&initialMarking, NULL)){
		return true;
	}

	//Main loop
	while(pwList->hasWaitingStates()){
		NonStrictMarking& next_marking = *pwList->getNextUnexplored();
                tmpParent = &next_marking;
		bool endOfMaxRun = true;
		if(!next_marking.meta->passed){
			NonStrictMarking marking(next_marking);
			next_marking.meta->passed = true;
			next_marking.meta->inTrace = true;
			trace.push(&next_marking);
			validChildren = 0;


			if(isDelayPossible(next_marking)){
				NonStrictMarking* marking = new NonStrictMarking(next_marking);
				marking->incrementAge();
				marking->setGeneratedBy(NULL);
                                if(addToPW(marking, &next_marking)){
                                        return true;
                                }
                                endOfMaxRun = false;
			}
                        if(successorGenerator.generateAndInsertSuccessors(next_marking)){
                                return true;
                        }
                        // if no delay is possible, and no transition-based succecors are possible, we have reached a max run
                        endOfMaxRun = endOfMaxRun && (!successorGenerator.doSuccessorsExist());


		} else {
			endOfMaxRun = false;
			validChildren = 0;
		}

		if(endOfMaxRun){
			std::cout << "End of max run" << std::endl;
			return true;
		}
		if(validChildren == 0){
			while(!trace.empty() && trace.top()->getNumberOfChildren() <= 1){
				trace.top()->meta->inTrace = false;
                                deleteMarking(trace.top());
				trace.pop();
			}
			if(trace.empty()){
                            return false;
                        }
                        trace.top()->decrementNumberOfChildren();
		}else{
			next_marking.setNumberOfChildren(validChildren);

		}
	}

	return false;
}

bool LivenessSearch::isDelayPossible(NonStrictMarking& marking){
	const PlaceList& places = marking.getPlaceList();
	if(places.size() == 0) return true;	//Delay always possible in empty markings

	PlaceList::const_iterator markedPlace_iter = places.begin();
	for(TAPN::TimedPlace::Vector::const_iterator place_iter = tapn->getPlaces().begin(); place_iter != tapn->getPlaces().end(); place_iter++){
		int inv = place_iter->get()->getInvariant().getBound();
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

bool LivenessSearch::addToPW(NonStrictMarking* marking, NonStrictMarking* parent){
	marking->cut();
	marking->setParent(parent);
	unsigned int size = marking->size();

	pwList->setMaxNumTokensIfGreater(size);

	if(size > options.getKBound()) {
		delete marking;
		return false;
	}

	QueryVisitor<NonStrictMarking> checker(*marking);
	boost::any context;
	query->accept(checker, context);
	if(!boost::any_cast<bool>(context))	return false;
	if(!pwList->add(marking)){
		//Test if collision is in trace
            if(marking->meta->inTrace){
                    //Make sure we can print trace
                    marking->setNumberOfChildren(1);
                    trace.push(marking);
                    return true;
            }else{
                    delete marking;
                    return false;
            }
	}else{
		validChildren++;
	}
        deleteMarking(marking);
	return false;
}

void LivenessSearch::printStats(){
	std::cout << "  discovered markings:\t" << pwList->discoveredMarkings << std::endl;
	std::cout << "  explored markings:\t" << pwList->size()-pwList->explored() << std::endl;
	std::cout << "  stored markings:\t" << pwList->size() << std::endl;
}

void LivenessSearch::getTrace(){
	stack < NonStrictMarking*> printStack;
	NonStrictMarking* m = trace.top();
	generateTraceStack(m, &printStack, &trace);
	if(options.getXmlTrace()){
		printXMLTrace(m, printStack, query->getQuantifier());
	} else {
		printHumanTrace(m, printStack, query->getQuantifier());
	}
}

LivenessSearch::~LivenessSearch() {
	// TODO Auto-generated destructor stub
}

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
