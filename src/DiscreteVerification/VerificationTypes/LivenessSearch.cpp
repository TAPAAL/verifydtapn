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
	: tapn(tapn), initialMarking(initialMarking), query(query), options(options), successorGenerator( *tapn.get() ){
}
    
LivenessSearch::LivenessSearch(boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<NonStrictMarking>* waiting_list)
	: pwList(new PWList(waiting_list, true)), tapn(tapn), initialMarking(initialMarking), query(query), options(options), successorGenerator( *tapn.get() ){
}

bool LivenessSearch::Verify(){
	if(addToPW(&initialMarking, NULL)){
		return true;
	}

	//Main loop
	while(pwList->HasWaitingStates()){
		NonStrictMarking& next_marking = *pwList->GetNextUnexplored();
		bool endOfMaxRun;
		if(!next_marking.meta->passed){
			NonStrictMarking marking(next_marking);
			endOfMaxRun = true;
			next_marking.meta->passed = true;
			next_marking.meta->inTrace = true;
			trace.push(&next_marking);
			validChildren = 0;

			// Generate next markings
			vector<NonStrictMarking*> next = getPossibleNextMarkings(next_marking);

			if(isDelayPossible(next_marking)){
				NonStrictMarking* marking = new NonStrictMarking(next_marking);
				marking->incrementAge();
				marking->SetGeneratedBy(NULL);
				next.push_back(marking);
			}

			for(vector<NonStrictMarking*>::iterator it = next.begin(); it != next.end(); it++){
				if(addToPW(*it, &next_marking)){
					return true;
				}
				endOfMaxRun = false;
			}
		} else {
			endOfMaxRun = false;
			validChildren = 0;
		}

		if(endOfMaxRun){
			std::cout << "End of max run" << std::endl;
			return true;
		}
		if(validChildren == 0){
			while(!trace.empty() && trace.top()->children <= 1){
				trace.top()->meta->inTrace = false;
                                deleteMarking(trace.top());
				trace.pop();
			}
			if(trace.empty()){
                            return false;
                        }
			trace.top()->children--;
		}else{
			next_marking.children = validChildren;

		}
	}

	return false;
}

bool LivenessSearch::isDelayPossible(NonStrictMarking& marking){
	PlaceList& places = marking.places;
	if(places.size() == 0) return true;	//Delay always possible in empty markings

	PlaceList::const_iterator markedPlace_iter = places.begin();
	for(TAPN::TimedPlace::Vector::const_iterator place_iter = tapn->GetPlaces().begin(); place_iter != tapn->GetPlaces().end(); place_iter++){
		int inv = place_iter->get()->GetInvariant().GetBound();
		if(*(place_iter->get()) == *(markedPlace_iter->place)){
			if(markedPlace_iter->MaxTokenAge() > inv-1){
				return false;
			}

			markedPlace_iter++;

			if(markedPlace_iter == places.end())	return true;
		}
	}
	assert(false);	// This happens if there are markings on places not in the TAPN
	return false;
}

vector<NonStrictMarking*> LivenessSearch::getPossibleNextMarkings(const NonStrictMarking& marking){
	return successorGenerator.generateSuccessors(marking);
}

bool LivenessSearch::addToPW(NonStrictMarking* marking, NonStrictMarking* parent){
	marking->cut();
	marking->SetParent(parent);
	unsigned int size = marking->size();

	pwList->SetMaxNumTokensIfGreater(size);

	if(size > options.GetKBound()) {
		delete marking;
		return false;
	}

	QueryVisitor<NonStrictMarking> checker(*marking);
	boost::any context;
	query->Accept(checker, context);
	if(!boost::any_cast<bool>(context))	return false;
	if(!pwList->Add(marking)){
		//Test if collision is in trace
            if(marking->meta->inTrace){
                    //Make sure we can print trace
                    marking->children = 1;
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
	std::cout << "  explored markings:\t" << pwList->Size()-pwList->Explored() << std::endl;
	std::cout << "  stored markings:\t" << pwList->Size() << std::endl;
}

void LivenessSearch::GetTrace(){
	stack < NonStrictMarking*> printStack;
	NonStrictMarking* m = trace.top();
	GenerateTraceStack(m, &printStack, &trace);
	if(options.XmlTrace()){
		PrintXMLTrace(m, printStack, query->GetQuantifier());
	} else {
		PrintHumanTrace(m, printStack, query->GetQuantifier());
	}
}

LivenessSearch::~LivenessSearch() {
	// TODO Auto-generated destructor stub
}

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
