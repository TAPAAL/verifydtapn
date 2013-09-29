/*
 * NonStrictSearch.cpp
 *
 *  Created on: 26/04/2012
 *      Author: MathiasGS
 */

#include "WorkflowStrongSoundness.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

bool WorkflowStrongSoundnessReachability::verify(){
		if(addToPW(&initialMarking, NULL)){
			return true;
		}

		//Main loop
		while(pwList->hasWaitingStates()){
			NonStrictMarking& next_marking = *pwList->getNextUnexplored();
	                tmpParent = &next_marking;

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
	                //deleteMarking(&next_marking);

		}

		return false;
	}

	void WorkflowStrongSoundnessReachability::getTrace(){
		std::stack < NonStrictMarking*> printStack;
		NonStrictMarking* next = lastMarking;
		do{
			NonStrictMarking* parent = ((WorkflowStrongSoundnessMetaData*) next->meta)->parents->at(0);
			printStack.push(next);
			next = parent;

		}while(((WorkflowStrongSoundnessMetaData*)next->meta)->parents != NULL && !((WorkflowStrongSoundnessMetaData*)next->meta)->parents->empty());

		if(printStack.top() != next){
			printStack.push(next);
		}

		if(options.getXmlTrace()){
			printXMLTrace(lastMarking, printStack, query, tapn);
		} else {
			printHumanTrace(lastMarking, printStack, query->getQuantifier());
		}
	}


	bool WorkflowStrongSoundnessReachability::addToPW(NonStrictMarking* marking, NonStrictMarking* parent){
		marking->cut();
		marking->setParent(parent);

		unsigned int size = marking->size();

		pwList->setMaxNumTokensIfGreater(size);

		if(size > options.getKBound()) {
			delete marking;
			return false;
		}

		/* Handle max */
		// Map to existing marking if any
		NonStrictMarking* lookup = pwList->lookup(marking);
		if(lookup != NULL){
			marking = lookup;
		}else{
			marking->meta = new WorkflowStrongSoundnessMetaData();
		}

		WorkflowStrongSoundnessMetaData* meta = (WorkflowStrongSoundnessMetaData*)marking->meta;

		if(parent != NULL)	meta->parents->push_back(parent);

		if(!marking->getTokenList(timer->getIndex()).empty() &&
				(marking->getTokenList(timer->getIndex()).at(0).getAge() > max_value ||
						(marking->getTokenList(timer->getIndex()).at(0).getAge() == max_value &&
								(!marking->getTokenList(term1->getIndex()).empty() || !marking->getTokenList(term2->getIndex()).empty())))){
			max_value = marking->getTokenList(timer->getIndex()).at(0).getAge();
			lastMarking = marking;
		}

		// Add to passed
		if(pwList->add(marking)){
			QueryVisitor<NonStrictMarking> checker(*marking, tapn);
			BoolResult context;

			query->accept(checker, context);
			if(context.value) {
				lastMarking = marking;
				return true;
			} else {
	                        //deleteMarking(marking);
				return false;
			}
		}

		return false;
	}

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
