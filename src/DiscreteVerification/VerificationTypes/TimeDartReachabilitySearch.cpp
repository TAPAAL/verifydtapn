/*
 * NonStrictSearch.cpp
 *
 *  Created on: 26/04/2012
 *      Author: MathiasGS
 */

#include "TimeDartReachabilitySearch.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

TimeDartReachabilitySearch::TimeDartReachabilitySearch(boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<TimeDart>* waiting_list)
	: pwList(waiting_list), tapn(tapn), initialMarking(initialMarking), query(query), options(options), successorGenerator( *tapn.get() ), allwaysEnabled(){

	//Find the transitions which don't have input arcs
	for(TimedTransition::Vector::const_iterator iter = tapn->GetTransitions().begin(); iter != tapn->GetTransitions().end(); iter++){
		if((*iter)->GetPreset().size() + (*iter)->GetTransportArcs().size() == 0){
			allwaysEnabled.push_back(iter->get());
		}
	}
}

bool TimeDartReachabilitySearch::Verify(){
	if(addToPW(&initialMarking, 0, INT_MAX)){
		return true;
	}

	//Main loop
	while(pwList.HasWaitingStates()){
		TimeDart& dart = *pwList.GetNextUnexplored();
		//std::cout << "Marking: " << dart.getBase() << " waiting: " << dart.getWaiting() << " passed: " << dart.getPassed() << std::endl;
		int passed = dart.getPassed();
		dart.setPassed(dart.getWaiting());
		vector<const TimedTransition*> transitions = getTransitions(&(dart.getBase()));
		for(vector<const TimedTransition*>::const_iterator transition = transitions.begin(); transition != transitions.end(); transition++){
			int calculatedStart = calculateStart(*(*transition), dart.getBase());
			if(calculatedStart == -1){	// Transition cannot be enabled in marking
				continue;
			}
			int start = max(dart.getWaiting(), calculatedStart);
			int end = min(passed, calculateEnd(*(*transition), dart.getBase()));
			if(start <= end){
				if((*transition)->GetPostsetSize() == 0){
					NonStrictMarking Mpp(dart.getBase());
					Mpp.incrementAge(start);
					vector<NonStrictMarking> next = getPossibleNextMarkings(Mpp, *(*transition));
					for(vector<NonStrictMarking>::iterator it = next.begin(); it != next.end(); it++){
						if(addToPW(&(*it), start, INT_MAX)){
							return true;
						}
					}
				}else{
					for(int n = start; n <= end; n++){
						NonStrictMarking Mpp(dart.getBase());
						Mpp.incrementAge(n);
						vector<NonStrictMarking> next = getPossibleNextMarkings(Mpp, **transition);
						for(vector<NonStrictMarking>::iterator it = next.begin(); it != next.end(); it++){
							if(addToPW(&(*it), 0, INT_MAX)){
								return true;
							}
						}
					}
				}
			}
		}
	}

	return false;
}

bool TimeDartReachabilitySearch::isDelayPossible(NonStrictMarking& marking){
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

vector<NonStrictMarking> TimeDartReachabilitySearch::getPossibleNextMarkings(NonStrictMarking& marking, const TimedTransition& transition){
	return successorGenerator.generateSuccessors(marking, transition);
}

bool TimeDartReachabilitySearch::addToPW(NonStrictMarking* marking, int w, int p){
	unsigned int size = marking->size();

	pwList.SetMaxNumTokensIfGreater(size);

	if(size > options.GetKBound()) {
		return false;
	}

	//TODO avoid copying
	marking = cut(*marking);

	TimeDart* dart = new TimeDart(marking, w, p);
	if(pwList.Add(dart)){
		QueryVisitor checker(*marking);
		boost::any context;
		query->Accept(checker, context);
		if(boost::any_cast<bool>(context)) {
			lastMarking = marking;
			return true;
		} else {
			return false;
		}
	}

	return false;
}

bool compare( const TimedTransition* lx, const TimedTransition* rx ) {
	return lx->GetIndex() < rx->GetIndex();
}

vector<const TimedTransition*> TimeDartReachabilitySearch::getTransitions(NonStrictMarking* marking){
	vector<const TimedTransition*> transitions;

	// TODO nicer?
	for(TimedTransition::Vector::const_iterator iter = tapn->GetTransitions().begin(); iter != tapn->GetTransitions().end(); iter++){
		transitions.push_back(iter->get());
	}

	return transitions;

	// Go through places
	for(vector<Place>::const_iterator place_iter = marking->places.begin(); place_iter != marking->places.end(); place_iter++){
		// Normal arcs
		for(TAPN::TimedInputArc::WeakPtrVector::const_iterator arc_iter = place_iter->place->GetInputArcs().begin(); arc_iter != place_iter->place->GetInputArcs().end(); arc_iter++){
			transitions.push_back(&arc_iter->lock()->OutputTransition());
		}
		// Transport arcs
		for(TAPN::TransportArc::WeakPtrVector::const_iterator arc_iter = place_iter->place->GetTransportArcs().begin(); arc_iter != place_iter->place->GetTransportArcs().end(); arc_iter++){
			transitions.push_back(&arc_iter->lock()->Transition());
		}
	}

	std::sort(transitions.begin(), transitions.end(), compare);
	transitions.erase(std::unique(transitions.begin(), transitions.end()), transitions.end());

	transitions.insert(transitions.end(), allwaysEnabled.begin(), allwaysEnabled.end());

	return transitions;
}

int TimeDartReachabilitySearch::calculateStart(const TimedTransition& transition, NonStrictMarking& marking){
	vector<Util::interval > start;
	Util::interval initial(0, INT_MAX);
	start.push_back(initial);

	// TODO do the same for transport arcs

	for(TAPN::TimedInputArc::WeakPtrVector::const_iterator arc = transition.GetPreset().begin(); arc != transition.GetPreset().end(); arc++){
		vector<Util::interval > intervals;
		int range;
		if(arc->lock()->Interval().GetUpperBound() == INT_MAX){
			range = INT_MAX;
		}else{
			range = arc->lock()->Interval().GetUpperBound()-arc->lock()->Interval().GetLowerBound();
		}
		int weight = arc->lock()->GetWeight();

		//TODO: Here we search twice
		int end = marking.NumberOfTokensInPlace(arc->lock()->InputPlace().GetIndex())-weight;
		TokenList tokens = marking.GetTokenList(arc->lock()->InputPlace().GetIndex());
		if(tokens.size() == 0) return -1;

		// TODO always enabled transitions?
		unsigned int j = 0;
		int numberOfTokensAvailable = tokens.at(j).getCount();
		for(unsigned int  i = 0; i < tokens.size(); i++){
			if(numberOfTokensAvailable < weight){
				for(j=max(i,j); j < tokens.size() && numberOfTokensAvailable < weight; j++){
					numberOfTokensAvailable += tokens.at(j).getCount();
				}
				j--;
			}
			if(numberOfTokensAvailable >= weight && tokens.at(j).getAge() - tokens.at(i).getAge() <= range){ //This span is interesting
				Util::interval element(arc->lock()->Interval().GetLowerBound() - tokens.at(i).getAge(),
						arc->lock()->Interval().GetUpperBound() - tokens.at(j).getAge());
				Util::set_add(intervals, element);
			}
			numberOfTokensAvailable -= tokens.at(i).getCount();
		}

		start = Util::setIntersection(start, intervals);
	}

	// Transport arcs
	for(TAPN::TransportArc::WeakPtrVector::const_iterator arc = transition.GetTransportArcs().begin(); arc != transition.GetTransportArcs().end(); arc++){
			vector<Util::interval > intervals;
			int range;
			if(arc->lock()->Interval().GetUpperBound() == INT_MAX){
				range = INT_MAX;
			}else{
				range = arc->lock()->Interval().GetUpperBound()-arc->lock()->Interval().GetLowerBound();
			}
			int weight = arc->lock()->GetWeight();

			//TODO: Here we search twice
			int end = marking.NumberOfTokensInPlace(arc->lock()->Source().GetIndex())-weight;
			TokenList tokens = marking.GetTokenList(arc->lock()->Source().GetIndex());

			if(tokens.size() == 0) return -1;

			// TODO always enabled transitions?
			unsigned int j = 0;
			int numberOfTokensAvailable = tokens.at(j).getCount();
			for(unsigned int  i = 0; i < tokens.size(); i++){
				if(numberOfTokensAvailable < weight){
					for(j=max(i,j); j < tokens.size() && numberOfTokensAvailable < weight; j++){
						numberOfTokensAvailable += tokens.at(j).getCount();
					}
					j--;
				}
				if(numberOfTokensAvailable >= weight && tokens.at(j).getAge() - tokens.at(i).getAge() <= range){ //This span is interesting
					Util::interval element(arc->lock()->Interval().GetLowerBound() - tokens.at(i).getAge(),
							arc->lock()->Interval().GetUpperBound() - tokens.at(j).getAge());
					Util::set_add(intervals, element);
				}
				numberOfTokensAvailable -= tokens.at(i).getCount();
			}

			start = Util::setIntersection(start, intervals);
		}


	return start.empty()? -1:start.at(0).lower();
}

int TimeDartReachabilitySearch::calculateEnd(const TimedTransition& transition, NonStrictMarking& marking){

	int part1 = 0;

	// Normal arcs
	for(TimedInputArc::WeakPtrVector::const_iterator iter = transition.GetPreset().begin(); iter != transition.GetPreset().end(); iter++){
		int c = iter->lock()->InputPlace().GetMaxConstant()+1-marking.GetTokenList(iter->lock()->InputPlace().GetIndex()).front().getAge();
		if(c > part1){
			part1 = c;
		}
	}

	// Transport arcs
	for(TransportArc::WeakPtrVector::const_iterator iter = transition.GetTransportArcs().begin(); iter != transition.GetTransportArcs().end(); iter++){
		int c = iter->lock()->Source().GetMaxConstant()+1-marking.GetTokenList(iter->lock()->Source().GetIndex()).front().getAge();
		if(c > part1){
			part1 = c;
		}
	}

	int part2 = INT_MAX;

	for(PlaceList::const_iterator iter = marking.GetPlaceList().begin(); iter != marking.GetPlaceList().end(); iter++){
		if(iter->place->GetInvariant().GetBound()-iter->tokens.back().getAge() < part2){
			part2 = iter->place->GetInvariant().GetBound()-iter->tokens.back().getAge();
		}
	}

	return min(part1, part2);
}

NonStrictMarking* TimeDartReachabilitySearch::cut(NonStrictMarking& marking){
	NonStrictMarking* m = new NonStrictMarking(marking);
	for(PlaceList::iterator place_iter = m->places.begin(); place_iter != m->places.end(); place_iter++){
		const TimedPlace& place = tapn->GetPlace(place_iter->place->GetIndex());
		//remove dead tokens
		if (place_iter->place->GetType() == Dead) {
			for(TokenList::iterator token_iter = place_iter->tokens.begin(); token_iter != place_iter->tokens.end(); token_iter++){
				if(token_iter->getAge() > place.GetMaxConstant()){
					token_iter->remove(token_iter->getCount());
				}
			}
		}
		//set age of too old tokens to max age
		int count = 0;
		for(TokenList::iterator token_iter = place_iter->tokens.begin(); token_iter != place_iter->tokens.end(); token_iter++){
			if(token_iter->getAge() > place.GetMaxConstant()){
				TokenList::iterator beginDelete = token_iter;
				if(place.GetType() == Std){
					for(; token_iter != place_iter->tokens.end(); token_iter++){
						count += token_iter->getCount();
					}
				}
				m->RemoveRangeOfTokens(*place_iter, beginDelete, place_iter->tokens.end());
				break;
			}
		}
		Token t(place.GetMaxConstant()+1,count);
		m->AddTokenInPlace(*place_iter, t);
	}
	m->CleanUp();
	return m;
}

void TimeDartReachabilitySearch::printStats(){
	std::cout << "  discovered markings:\t" << pwList.discoveredMarkings << std::endl;
	std::cout << "  explored markings:\t" << pwList.Size()-pwList.waiting_list->Size() << std::endl;
	std::cout << "  stored markings:\t" << pwList.Size() << std::endl;
}

TimeDartReachabilitySearch::~TimeDartReachabilitySearch() {
}

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
