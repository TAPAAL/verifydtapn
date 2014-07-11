/*
 * NonStrictSearch.cpp
 *
 *  Created on: 26/04/2012
 *      Author: MathiasGS
 */

#include "LivenessSearch.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

LivenessSearch::LivenessSearch(TAPN::TimedArcPetriNet& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options)
	: AbstractNaiveVerification<PWListBase,NonStrictMarking>(tapn, initialMarking, query, options, NULL){

}
    
LivenessSearch::LivenessSearch(TAPN::TimedArcPetriNet& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<NonStrictMarking>* waiting_list)
	: AbstractNaiveVerification<PWListBase,NonStrictMarking>(tapn, initialMarking, query, options, new PWList(waiting_list, true)){

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

                        bool noDelay = false;
                        Result res = successorGenerator.generateAndInsertSuccessors(next_marking);
                        if (res == ADDTOPW_RETURNED_TRUE) {
                            return true;
                        } else if (res == ADDTOPW_RETURNED_FALSE_URGENTENABLED) {
                            noDelay = true;
                        }
                        
                        
			if(!noDelay && isDelayPossible(next_marking)){
				NonStrictMarking* marking = new NonStrictMarking(next_marking);
				marking->incrementAge();
				marking->setGeneratedBy(NULL);
                                if(addToPW(marking, &next_marking)){
                                        return true;
                                }
                                endOfMaxRun = false;
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

bool LivenessSearch::addToPW(NonStrictMarking* marking, NonStrictMarking* parent){
	marking->cut(placeStats);
	marking->setParent(parent);
	unsigned int size = marking->size();

	pwList->setMaxNumTokensIfGreater(size);

	if(size > options.getKBound()) {
		delete marking;
		return false;
	}

	QueryVisitor<NonStrictMarking> checker(*marking, tapn);
	AST::BoolResult context;
	query->accept(checker, context);
	if(!context.value){
            delete marking;
            return false;
        }
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
                deleteMarking(marking);
		validChildren++;
                return false;
	}

}

void LivenessSearch::getTrace(){
	stack < NonStrictMarking*> printStack;
	NonStrictMarking* m = trace.top();
	generateTraceStack(m, &printStack, &trace);
	if(options.getXmlTrace()){
		printXMLTrace(m, printStack, query, tapn);
	} else {
		printHumanTrace(m, printStack, query->getQuantifier());
	}
}

LivenessSearch::~LivenessSearch() {

}

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
