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
	: pwList(waiting_list), tapn(tapn), initialMarking(initialMarking), query(query), options(options), successorGenerator( *tapn.get() ){
}

bool TimeDartReachabilitySearch::Verify(){
	if(addToPW(&initialMarking, 0, INT_MAX)){
		return true;
	}

	//Main loop
	while(pwList.HasWaitingStates()){
		TimeDart& dart = *pwList.GetNextUnexplored();
		bool endOfMaxRun = true;

		vector<TimedTransition> transitions = getTransitions(&(dart.getBase()));
		for(vector<TimedTransition>::const_iterator transition = transitions.begin(); transition != transitions.end(); transition++){
			int calculatedStart = calculateStart((*transition), dart.getBase());
			int start = max(dart.getWaiting(), calculatedStart);
			int end = min(dart.getPassed(), calculateEnd((*transition), dart.getBase()));
			if(start <= end){
				if(transition->GetPostsetSize() == 0){
					// TODO DO THIS FOR ALL GENERATEABLE MARKINGS, NOT FOR SAME BASE MARKING
					if(addToPW(&(dart.getBase()), start, INT_MAX)){
						return true;
					}
				}else{
					for(int n = start; n < end; n++){
						// TODO DO THIS FOR ALL GENERATEABLE MARKINGS, NOT FOR SAME BASE MARKING
						if(addToPW(&(dart.getBase()), 0, INT_MAX)){
							return true;
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

vector<NonStrictMarking> TimeDartReachabilitySearch::getPossibleNextMarkings(NonStrictMarking& marking){
	return successorGenerator.generateSuccessors(marking);
}

bool TimeDartReachabilitySearch::addToPW(NonStrictMarking* marking, int w, int p){
	unsigned int size = marking->size();

	pwList.SetMaxNumTokensIfGreater(size);

	if(size > options.GetKBound()) {
		return false;
	}

	TimeDart dart = makeDart(marking, w, p);
	if(pwList.Add(&dart)){
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

TimeDart TimeDartReachabilitySearch::makeDart(NonStrictMarking* marking, int w, int p){
	TimeDart* dart = new TimeDart(marking, w, p);
	return *dart;
}

bool compare( const TimedTransition& lx, const TimedTransition& rx ) {
	return lx.GetIndex() < rx.GetIndex();
}

vector<TimedTransition> TimeDartReachabilitySearch::getTransitions(NonStrictMarking* marking){
	vector<TimedTransition>* transitions = new vector<TimedTransition>();

	// Go through places
	for(vector<Place>::const_iterator place_iter = marking->places.begin(); place_iter != marking->places.end(); place_iter++){
		// Normal arcs
		for(TAPN::TimedInputArc::WeakPtrVector::const_iterator arc_iter = place_iter->place->GetInputArcs().begin(); arc_iter != place_iter->place->GetInputArcs().end(); arc_iter++){
			transitions->push_back(arc_iter->lock()->OutputTransition());
		}
		// Transport arcs
		for(TAPN::TransportArc::WeakPtrVector::const_iterator arc_iter = place_iter->place->GetTransportArcs().begin(); arc_iter != place_iter->place->GetTransportArcs().end(); arc_iter++){
			transitions->push_back(arc_iter->lock()->Transition());
		}
	}

	std::sort(transitions->begin(), transitions->end(), compare);
	transitions->erase(std::unique(transitions->begin(), transitions->end()), transitions->end());

	return *transitions;
}

void set_add(vector< pair<int, int> >* first, pair<int, int>* element){

	bool inserted = false;
	for(unsigned int i = 0; i < first->size(); i++){

		if(!inserted){
			if(element->second < first->at(i).first){
				// Add element
				first->insert(first->begin()+i, *element);
				inserted = true;
			}else if(element->first >= first->at(i).first && element->first <= first->at(i).second){
				// Merge with node
				int _max = max(first->at(i).second, element->second);
				first->at(i).second = _max;
				inserted = true;
				// Clean up
				for(i += 1; i < first->size(); i++){
					if(first->at(i).first <= _max){
						// Merge items
						if(first->at(i).second >= _max){
							first->at(i-1).second = first->at(i).second;
						}
						first->erase(first->begin()+i-1,first->begin()+i);
						i--;
					}
				}
			}
		}else{
			break;
		}
	}

	if(!inserted){
		first->push_back(*element);
	}
}

int TimeDartReachabilitySearch::calculateStart(const TimedTransition& transition, NonStrictMarking& marking){
	vector<pair<int, int> >* start = new vector<pair<int, int> >();
	pair<int, int>* initial = new pair<int, int>(0, INT_MAX);
	start->push_back(*initial);

	// TODO do the same for transport arcs

	for(TAPN::TimedInputArc::WeakPtrVector::const_iterator arc = transition.GetPreset().begin(); arc != transition.GetPreset().end(); arc++){
		vector<pair<int, int> >* intervals = new vector<pair<int, int> >();
		int range = 0;
		if(arc->lock()->Interval().GetUpperBound() == INT_MAX){
			range = INT_MAX;
		}else{
			range = arc->lock()->Interval().GetUpperBound()-arc->lock()->Interval().GetLowerBound();
		}
		int weight = arc->lock()->GetWeight();

		int end = marking.NumberOfTokensInPlace(arc->lock()->InputPlace().GetIndex())-weight;
		TokenList tokens = marking.GetTokenList(arc->lock()->InputPlace().GetIndex());
		int ii = 0;
		for(int i = 0; i <= end; i++){
			for(int c = 0; c < tokens.at(ii).getCount(); c++){
				pair<int, int>* element = new pair<int, int>(0,0);
				set_add(intervals, element);
			}
			ii++;
		}
		// TODO intersection of intervals and start
	}
	return start->at(0).first;
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
