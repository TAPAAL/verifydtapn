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
	std::vector<TAPN::TimedTransition> enabledTransitions;

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
		std::vector<TAPN::TimedTransition>& enabledTransitions,
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
		enabledTransitions.push_back(transition);
	}
}

/*
vector< NonStrictMarking > SuccessorGenerator::generateSuccessors2(const NonStrictMarking& marking) const{
#if DEBUG
	std::cout << std::endl << "Successor generator:" << std::endl << "------------------------" << std::endl;
#endif
	vector< NonStrictMarking > result;

	/* Check enabled transitions
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
*/


TokenList SuccessorGenerator::getPlaceFromMarking(const NonStrictMarking& marking, int placeID) const{
	for(PlaceList::const_iterator iter = marking.GetPlaceList().begin();
			iter != marking.GetPlaceList().end();
			iter++){
		if(iter->place->GetIndex() == placeID) return iter->tokens;
	}
	return TokenList();
}

void SuccessorGenerator::generateMarkings(vector<NonStrictMarking>& result, const NonStrictMarking& init_marking,
		const std::vector< TimedTransition >& transitions, ArcHashMap& enabledArcs) const {

	for(std::vector< TimedTransition >::const_iterator iter = transitions.begin(); iter != transitions.end(); iter++){
		bool inhibited = false;
		//Check that no inhibitors is enabled;
#if DEBUG
		std::cout << "Transition: " << *iter << "Number of inhibitors: " << iter->GetInhibitorArcs().size() << std::endl;
#endif

		for(TAPN::InhibitorArc::WeakPtrVector::const_iterator inhib_iter = iter->GetInhibitorArcs().begin(); inhib_iter != iter->GetInhibitorArcs().end(); inhib_iter++){
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
		recursiveGenerateMarking(result, m, *iter, enabledArcs, 0);
	}
}

void SuccessorGenerator::recursiveGenerateMarking(vector<NonStrictMarking>& result, NonStrictMarking& init_marking, const TimedTransition& transition, ArcHashMap& enabledArcs, unsigned int index) const{
#if DEBUG
	std::cout << "Transition: " << transition << " Num of input arcs: " << transition.GetPreset().size() << " Num of transport arcs: " << transition.GetTransportArcs().size() << std::endl;
#endif
	if(index != transition.GetPreset().size()+transition.GetTransportArcs().size()){
		//Not the last index
		if(index < transition.GetPreset().size()){
			TokenList tokens = enabledArcs[transition.GetPreset().at(index).lock().get()];
			for(TokenList::iterator it =  tokens.begin(); it != tokens.end(); it++){
				NonStrictMarking marking(init_marking);
#if DEBUG
				std::cout << "Before: " << marking << std::endl;
#endif
				marking.RemoveToken(transition.GetPreset().at(index).lock()->InputPlace().GetIndex(), it->getAge());
#if DEBUG
				std::cout << "After: " << marking << std::endl;
#endif
				recursiveGenerateMarking(result, marking, transition, enabledArcs, index+1);
			}
		}
		if(index >= transition.GetPreset().size() && index < transition.GetPreset().size() + transition.GetTransportArcs().size()){
			TokenList tokens = enabledArcs[transition.GetTransportArcs().at(index-transition.GetPreset().size()).lock().get()];
			for(TokenList::iterator it = tokens.begin();
					it != tokens.end();
					it++){
				NonStrictMarking marking(init_marking);

				marking.RemoveToken(transition.GetTransportArcs().at(index-transition.GetPreset().size()).lock()->Source().GetIndex(), it->getAge());
				marking.AddTokenInPlace(transition.GetTransportArcs().at(index-transition.GetPreset().size()).lock()->Destination(), it->getAge());
				recursiveGenerateMarking(result, marking, transition, enabledArcs, index+1);
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
