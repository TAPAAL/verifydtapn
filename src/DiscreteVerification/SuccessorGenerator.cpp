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
	std::cout << "------------------------------- SuccessorGenerator ---------------------------" << std::endl;
#endif
	vector< NonStrictMarking > result;
	ArcHashMap enabledArcs(tapn.GetInhibitorArcs().size() + tapn.GetInputArcs().size() + tapn.GetTransportArcs().size());
	std::vector<unsigned int> enabledTransitionArcs(tapn.GetTransitions().size(), 0);
	std::vector<const TAPN::TimedTransition* > enabledTransitions;

	//Find the transitions which don't have input arcs
	for(TimedTransition::Vector::const_iterator iter = tapn.GetTransitions().begin(); iter != tapn.GetTransitions().end(); iter++){
		if((*iter)->GetPreset().size() + (*iter)->GetTransportArcs().size() == 0){
			enabledTransitions.push_back(iter->get());
		}
	}

	for(PlaceList::const_iterator iter = marking.places.begin(); iter < marking.places.end(); iter++){
		for(TAPN::TimedInputArc::WeakPtrVector::const_iterator arc_iter = iter->place->GetInputArcs().begin();
				arc_iter != iter->place->GetInputArcs().end(); arc_iter++){
			processArc(enabledArcs, enabledTransitionArcs, enabledTransitions,
					*iter, arc_iter->lock()->Interval(), arc_iter->lock().get(), arc_iter->lock()->OutputTransition());
		}

		for(TAPN::TransportArc::WeakPtrVector::const_iterator arc_iter = iter->place->GetTransportArcs().begin();
				arc_iter != iter->place->GetTransportArcs().end(); arc_iter++){
			processArc(enabledArcs, enabledTransitionArcs, enabledTransitions,
					*iter, arc_iter->lock()->Interval(), arc_iter->lock().get(),
					arc_iter->lock()->Transition(), arc_iter->lock()->Destination().GetInvariant().GetBound());
		}

		for(TAPN::InhibitorArc::WeakPtrVector::const_iterator arc_iter = iter->place->GetInhibitorArcs().begin();
				arc_iter != iter->place->GetInhibitorArcs().end(); arc_iter++){
			TimeInterval t(false, 0, std::numeric_limits<int>().max(), true);
			processArc(enabledArcs, enabledTransitionArcs, enabledTransitions,
					*iter, t, arc_iter->lock().get(), arc_iter->lock()->OutputTransition(), std::numeric_limits<int>().max(), true);
		}
	}

	generateMarkings(result, marking, enabledTransitions, enabledArcs);
#if DEBUG
	std::cout << "------------------------------- SuccessorGenerator done---------------------------" << std::endl;
#endif
	return result;
}

void SuccessorGenerator::processArc(
		ArcHashMap& enabledArcs,
		std::vector<unsigned int>& enabledTransitionArcs,
		std::vector<const TAPN::TimedTransition* >& enabledTransitions,
		const Place& place,
		const TAPN::TimeInterval& interval,
		const void* arcAddress,
		const TimedTransition& transition,
		int bound,
		bool isInhib
) const{
	bool arcIsEnabled = false;
#if DEBUG
	std::cout << "Bound: " << bound << std::endl;
#endif
	for(TokenList::const_iterator token_iter = place.tokens.begin(); token_iter != place.tokens.end(); token_iter++){
		if(interval.GetLowerBound() <= token_iter->getAge() && token_iter->getAge() <= interval.GetUpperBound() && token_iter->getAge() <= bound){
			enabledArcs[arcAddress].push_back(*token_iter);
			arcIsEnabled = true;
#if DEBUG
			std::cout << "Arc enabled!" << std::endl;
#endif
		}
	}
	if(arcIsEnabled && !isInhib){
		enabledTransitionArcs[transition.GetIndex()]++;
	}
	if(enabledTransitionArcs[transition.GetIndex()] == transition.GetPreset().size() + transition.GetTransportArcs().size() && !isInhib){
#if DEBUG
		std::cout << "Transition pushed: " << transition << std::endl;
#endif
		enabledTransitions.push_back(&transition);
	}
}


TokenList SuccessorGenerator::getPlaceFromMarking(const NonStrictMarking& marking, int placeID) const{
	for(PlaceList::const_iterator iter = marking.GetPlaceList().begin();
			iter != marking.GetPlaceList().end();
			iter++){
		if(iter->place->GetIndex() == placeID) return iter->tokens;
	}
	return TokenList();
}

void SuccessorGenerator::generateMarkings(vector<NonStrictMarking>& result, const NonStrictMarking& init_marking,
		const std::vector< const TimedTransition* >& transitions, ArcHashMap& enabledArcs) const {

	for(std::vector< const TimedTransition* >::const_iterator iter = transitions.begin(); iter != transitions.end(); iter++){
		bool inhibited = false;
		//Check that no inhibitors is enabled;
#if DEBUG
		std::cout << "Transition: " << *(*iter) << "Number of inhibitors: " << iter->GetInhibitorArcs().size() << std::endl;
#endif

		for(TAPN::InhibitorArc::WeakPtrVector::const_iterator inhib_iter = (*iter)->GetInhibitorArcs().begin(); inhib_iter != (*iter)->GetInhibitorArcs().end(); inhib_iter++){
#if DEBUG
			std::cout << "Inhibitor: " << *(inhib_iter->lock()) << " Inhibitor size: " << enabledArcs[inhib_iter->lock().get()].size() << std::endl;
#endif
			if(enabledArcs[inhib_iter->lock().get()].size() != 0){
				inhibited = true;
				break;
			}
		}
#if DEBUG
		std::cout << "No inhibitors activated" << std::endl;
#endif
		if (inhibited) continue;
		NonStrictMarking m(init_marking);
		m.SetGeneratedBy(*iter);
		recursiveGenerateMarking(result, m, *(*iter), enabledArcs, 0);
	}
}


void SuccessorGenerator::recursiveGenerateMarking(vector<NonStrictMarking>& result, NonStrictMarking& init_marking, const TimedTransition& transition, ArcHashMap& enabledArcs, unsigned int index) const{
#if DEBUG
	std::cout << "Transition: " << transition << " Num of input arcs: " << transition.GetPreset().size() << " Num of transport arcs: " << transition.GetTransportArcs().size() << std::endl;
#endif
	if(index != transition.GetPreset().size()+transition.GetTransportArcs().size()){
		//Not the last index
		if(index < transition.GetPreset().size()){
			vector< NonStrictMarking > permultations;

			generatePermultations(permultations, init_marking, transition.GetPreset().at(index).lock()->InputPlace().GetIndex(), enabledArcs[transition.GetPreset().at(index).lock().get()], 0, transition.GetPreset().at(index).lock()->GetWeight());

			for(vector< NonStrictMarking >::iterator it = permultations.begin(); it != permultations.end(); it++){

				recursiveGenerateMarking(result, *it, transition, enabledArcs, index+1);

			}

		}
		if(index >= transition.GetPreset().size() && index < transition.GetPreset().size() + transition.GetTransportArcs().size()){
			vector< NonStrictMarking > permultations;

			generatePermultations(permultations, init_marking, transition.GetTransportArcs().at(index-transition.GetPreset().size()).lock()->Source().GetIndex(),
					enabledArcs[transition.GetTransportArcs().at(index-transition.GetPreset().size()).lock().get()], 0,
					transition.GetTransportArcs().at(index-transition.GetPreset().size()).lock()->GetWeight(),
					&(transition.GetTransportArcs().at(index-transition.GetPreset().size()).lock()->Destination()));

			for(vector< NonStrictMarking >::iterator it = permultations.begin(); it != permultations.end(); it++){
				recursiveGenerateMarking(result, *it, transition, enabledArcs, index+1);
			}
		}
	}else{
		//Beyond last index
		for(OutputArc::WeakPtrVector::const_iterator it = transition.GetPostset().begin(); it != transition.GetPostset().end(); it++){
			Token t(0, it->lock()->GetWeight());
			init_marking.AddTokenInPlace(it->lock().get()->OutputPlace(), t);
		}
		result.push_back(init_marking);
	}
}

void SuccessorGenerator::generatePermultations(vector< NonStrictMarking >& result, NonStrictMarking& init_marking, int placeID, TokenList enabledBy, int tokenToProcess, int remainingToRemove, TimedPlace* destinationPlace) const{
	if(remainingToRemove == 0){
		result.push_back(init_marking);
		return;
	} else if (tokenToProcess >= enabledBy.size()){
		return;
	}

	bool breakouter = false;
	int age = enabledBy[tokenToProcess].getAge();
	for(int i = 0; i <= remainingToRemove; i++){
		NonStrictMarking marking(init_marking);
		for(int j = 0; j < i; j++){
			if(!marking.RemoveToken(placeID, age)){
				breakouter = true;
				break;
			}

			if (destinationPlace) {
			    // Input arc is a transport arc
				Token t(age, j);
				marking.AddTokenInPlace(*destinationPlace, t);
			}
		}
		if(breakouter) break;
		generatePermultations(result, marking, placeID, enabledBy, tokenToProcess+1, remainingToRemove-i, destinationPlace);
	}
}


} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
