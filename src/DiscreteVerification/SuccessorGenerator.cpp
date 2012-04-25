/*
 * SuccessorGenerator.cpp
 *
 *  Created on: 22/03/2012
 *      Author: MathiasGS
 */

#include "SuccessorGenerator.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

SuccessorGenerator::~SuccessorGenerator(){

}

vector< NonStrictMarking > SuccessorGenerator::generateSuccessors(const NonStrictMarking& marking) const{
#if DEBUG
	std::cout << std::endl << "Successor generator:" << std::endl << "------------------------" << std::endl;
#endif
	vector< NonStrictMarking > result;

	/* Check enabled transitions*/
	for(TimedTransition::Vector::const_iterator trans_iter = tapn.GetTransitions().begin(); trans_iter != tapn.GetTransitions().end(); trans_iter++){
		vector< InputArcRef > inputArcs;
		vector< TransportArcRef > transportArcs;
		bool continue_outer = false;

		//Inhib
		for(InhibitorArc::WeakPtrVector::const_iterator it = trans_iter->get()->GetInhibitorArcs().begin();
				it != trans_iter->get()->GetInhibitorArcs().end();
				it++){
			TokenList tokens = getPlaceFromMarking(marking, it->lock()->InputPlace().GetIndex());

			if(tokens.size() > 0){
				continue_outer = true;
				break;
			}
		}
		if(continue_outer) continue;

		//Normal
		for(TimedInputArc::WeakPtrVector::const_iterator it = trans_iter->get()->GetPreset().begin();
				it != trans_iter->get()->GetPreset().end();
				it++){

			TokenList tokens = getPlaceFromMarking(marking, it->lock()->InputPlace().GetIndex());

			InputArcRef arcref(it->lock());
			for(TokenList::const_iterator t_it = tokens.begin(); t_it != tokens.end(); t_it++){
				if(t_it->getAge() >= it->lock()->Interval().GetLowerBound() && t_it->getAge() <= it->lock()->Interval().GetUpperBound()){
					arcref.enabledBy.push_back(*t_it);
				}
			}

			if(arcref.enabledBy.size() > 0) inputArcs.push_back(arcref);
			else	continue_outer = true;
		}
		if(continue_outer) continue;

		//Transport
		for(TransportArc::WeakPtrVector::const_iterator it = trans_iter->get()->GetTransportArcs().begin();
				it != trans_iter->get()->GetTransportArcs().end();
				it++){
			TokenList tokens = getPlaceFromMarking(marking, it->lock()->Source().GetIndex());

			TransportArcRef arcref(it->lock());
			for(TokenList::const_iterator t_it = tokens.begin(); t_it != tokens.end(); t_it++){
				if(t_it->getAge() >= it->lock()->Interval().GetLowerBound() &&
						t_it->getAge() <= it->lock()->Interval().GetUpperBound() &&
						t_it->getAge() <= it->lock()->Destination().GetInvariant().GetBound()){
					arcref.enabledBy.push_back(*t_it);
				}
			}

			if(arcref.enabledBy.size() > 0) transportArcs.push_back(arcref);
			else	continue_outer = true;
		}
		if(continue_outer) continue;

		generateMarkings(result, marking, *trans_iter->get(), inputArcs, transportArcs);
	}
#if DEBUG
	std::cout << "Size of result: " << result.size() << std::endl;

	std::cout << "------------------------" << std::endl << std::endl;
#endif
	return result;
}

TokenList SuccessorGenerator::getPlaceFromMarking(const NonStrictMarking& marking, int placeID) const{
	for(PlaceList::const_iterator iter = marking.GetPlaceList().begin();
			iter != marking.GetPlaceList().end();
			iter++){
		if(iter->place->GetIndex() == placeID) return iter->tokens;
	}
	return TokenList();
}

void SuccessorGenerator::generateMarkings(vector<NonStrictMarking>& result, const NonStrictMarking& init_marking, const TimedTransition& transition, vector<InputArcRef> inputArcs, vector<TransportArcRef> transportArcs) const{
	NonStrictMarking marking(init_marking);
	recursiveGenerateMarking(result, marking, transition, inputArcs, transportArcs, 0);
}

void SuccessorGenerator::recursiveGenerateMarking(vector<NonStrictMarking>& result, NonStrictMarking& init_marking, const TimedTransition& transition, vector<InputArcRef> inputArcs, vector<TransportArcRef> transportArcs, int index) const{
	if(index != inputArcs.size()+transportArcs.size()){
		//Not the last index
		if(index < inputArcs.size()){
			for(TokenList::iterator it = inputArcs.at(index).enabledBy.begin(); it != inputArcs.at(index).enabledBy.end(); it++){
				NonStrictMarking marking(init_marking);
#if DEBUG
				std::cout << "Before: " << marking << std::endl;
#endif
				marking.RemoveToken(inputArcs.at(index).arc.lock().get()->InputPlace().GetIndex(), it->getAge());
#if DEBUG
				std::cout << "After: " << marking << std::endl;
#endif
				recursiveGenerateMarking(result, marking, transition, inputArcs, transportArcs, index+1);
			}
		}
		if(index >= inputArcs.size() && index < inputArcs.size() + transportArcs.size()){
			for(TokenList::iterator it = transportArcs.at(index-inputArcs.size()).enabledBy.begin();
					it != transportArcs.at(index-inputArcs.size()).enabledBy.end();
					it++){
				NonStrictMarking marking(init_marking);

				marking.RemoveToken(transportArcs.at(index-inputArcs.size()).arc.lock().get()->Source().GetIndex(), it->getAge());
				marking.AddTokenInPlace(transportArcs.at(index-inputArcs.size()).arc.lock().get()->Destination(), it->getAge());
				recursiveGenerateMarking(result, marking, transition, inputArcs, transportArcs, index+1);
			}
		}
	}else{
		//Beyond last index
		for(OutputArc::WeakPtrVector::const_iterator it = transition.GetPostset().begin(); it != transition.GetPostset().end(); it++){
			init_marking.AddTokenInPlace(it->lock().get()->OutputPlace(), 0);
		}
		result.push_back(init_marking);
	}
}


} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
