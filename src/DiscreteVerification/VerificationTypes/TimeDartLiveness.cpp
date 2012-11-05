/*
 * NonStrictSearch.cpp
 *
 *  Created on: 26/04/2012
 *      Author: MathiasGS
 */

#include "TimeDartLiveness.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

TimeDartLiveness::TimeDartLiveness(boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<WaitingDart>* waiting_list)
: pwList(waiting_list), tapn(tapn), initialMarking(initialMarking), query(query), options(options), successorGenerator( *tapn.get() ), allwaysEnabled(), exploredMarkings(0), trace(){

	//Find the transitions which don't have input arcs
	for(TimedTransition::Vector::const_iterator iter = tapn->GetTransitions().begin(); iter != tapn->GetTransitions().end(); iter++){
		if((*iter)->GetPreset().size() + (*iter)->GetTransportArcs().size() == 0){
			allwaysEnabled.push_back(iter->get());
		}
	}
}

bool TimeDartLiveness::Verify(){
	if(addToPW(&initialMarking, NULL, 0, INT_MAX)){
		return true;
	}

	//Main loop
	while(pwList.HasWaitingStates()){
		WaitingDart& waitingDart = *pwList.GetNextUnexplored();
		exploredMarkings++;

#ifdef DEBUG
		std::cout << "-----------------------------------------------------------------------------\n";
		std::cout << "Marking: " << *(waitingDart.dart->getBase()) << " waiting: " << waitingDart.dart->getWaiting() << " passed: " << waitingDart.dart->getPassed() << std::endl;
#endif

		if(canDelayForever(waitingDart.dart->getBase())){
			return true;
		}

		int maxCalculatedEnd = -1;

		trace.push(new TraceDart(waitingDart.parent, waitingDart.start, waitingDart.end));

		int passed = waitingDart.dart->getPassed();
		waitingDart.dart->setPassed(waitingDart.w);
		tapn->GetTransitions();
		for(TimedTransition::Vector::const_iterator transition_iter = tapn->GetTransitions().begin();
				transition_iter != tapn->GetTransitions().end(); transition_iter++){
			TimedTransition& transition = **transition_iter;

#ifdef DEBUG
			std::cout << "Transition: " << transition << std::endl;
#endif

			pair<int,int> calculatedStart = calculateStart(transition, waitingDart.dart->getBase());
			if(calculatedStart.first == -1){	// Transition cannot be enabled in marking
				continue;
			}

			// Aggregate on calculatedEnd for deadlock detection
			if(calculatedStart.second > maxCalculatedEnd){
				maxCalculatedEnd = calculatedStart.second;
			}

			int start = max(waitingDart.w, calculatedStart.first);
			int end = min(passed-1, calculatedStart.second);
			if(start <= end){

				if(transition.GetPostset().size() == 0 || transition.hasUntimedPostset()){
					NonStrictMarking Mpp(*waitingDart.dart->getBase());
					Mpp.incrementAge(start);
					vector<NonStrictMarking*> next = getPossibleNextMarkings(Mpp, transition);
					for(vector<NonStrictMarking*>::iterator it = next.begin(); it != next.end(); it++){
						if(addToPW(*it, waitingDart.dart->getBase(), start, calculatedStart.second)){
							return true;
						}
					}
				}else{
					int stop = max(start, calculateStop(transition, waitingDart.dart->getBase()));
					int finalStop = min(stop, end);
					for(int n = start; n <= finalStop; n++){
						NonStrictMarking Mpp(*waitingDart.dart->getBase());
						Mpp.incrementAge(n);
						int _end = n;
						if(n == stop){
							_end = calculatedStart.second;
						}

						vector<NonStrictMarking*> next = getPossibleNextMarkings(Mpp, transition);
						for(vector<NonStrictMarking*>::iterator it = next.begin(); it != next.end(); it++){
							if(addToPW(*it, waitingDart.dart->getBase(), n, _end)){
								return true;
							}
						}
					}
				}
			}
		}

		if(maxCalculatedEnd < maxPossibleDelay(waitingDart.dart->getBase())){
			return true;	/* DEADLOCK! */
		}

		delete &waitingDart;

		while(!trace.empty() && trace.top()->successors == 0){
			TraceDart* tmp = trace.top();
			trace.pop();
			delete tmp;
			if(trace.empty()){
				return false;
			}
			trace.top()->successors--;
		}
	}

	return false;
}

bool TimeDartLiveness::canDelayForever(NonStrictMarking* marking){
	for(PlaceList::const_iterator p_iter = marking->GetPlaceList().begin(); p_iter != marking->GetPlaceList().end(); p_iter++){
		if(p_iter->place->GetInvariant().GetBound() < INT_MAX){
			return false;
		}
	}
	return true;
}

int TimeDartLiveness::maxPossibleDelay(NonStrictMarking* marking){
	int invariantPart = INT_MAX;

	for(PlaceList::const_iterator iter = marking->GetPlaceList().begin(); iter != marking->GetPlaceList().end(); iter++){
		if(iter->place->GetInvariant().GetBound() != std::numeric_limits<int>::max() && iter->place->GetInvariant().GetBound()-iter->tokens.back().getAge() < invariantPart){
			invariantPart = iter->place->GetInvariant().GetBound()-iter->tokens.back().getAge();
		}
	}

	return invariantPart;
}

bool TimeDartLiveness::isDelayPossible(NonStrictMarking& marking){
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

vector<NonStrictMarking*> TimeDartLiveness::getPossibleNextMarkings(NonStrictMarking& marking, const TimedTransition& transition){
	return successorGenerator.generateSuccessors(marking, transition);
}

bool TimeDartLiveness::addToPW(NonStrictMarking* marking, NonStrictMarking* parent, int start, int end){
	marking->cut();

	unsigned int size = marking->size();

	pwList.SetMaxNumTokensIfGreater(size);

	if(size > options.GetKBound()) {
		return false;
	}

	int youngest = marking->makeBase(tapn.get());

	// TODO optimize
	int loop = false;
	if(parent != NULL && parent->equals(*marking) && youngest <= end){
		loop = true;
	}

	stack< TraceDart* > tmp;
	while(!trace.empty() && trace.top()->parent != NULL){
		tmp.push(trace.top());
		if(trace.top()->parent->equals(*marking) && youngest <= trace.top()->end){
			loop = true;
		}
		trace.pop();
	}
	while(!tmp.empty()){
		trace.push(tmp.top());
		tmp.pop();
	}
	if(loop){
		lastMarking = marking;
		return true;
	}

	QueryVisitor checker(*marking);
	boost::any context;
	query->Accept(checker, context);
	if(boost::any_cast<bool>(context)) {
		if(pwList.Add(tapn.get(), marking, youngest, parent, start, end)){
			if(!trace.empty()){
				trace.top()->successors++;
			}
		}
	}

	return false;
}

pair<int,int> TimeDartLiveness::calculateStart(const TimedTransition& transition, NonStrictMarking* marking){
	vector<Util::interval > start;
	Util::interval initial(0, INT_MAX);
	start.push_back(initial);

	if(transition.NumberOfInputArcs() + transition.NumberOfTransportArcs() == 0){ //always enabled
		pair<int, int> p(0, INT_MAX);
		return p;
	}

	//inhibited
	for(TAPN::InhibitorArc::WeakPtrVector::const_iterator arc = transition.GetInhibitorArcs().begin();
			arc != transition.GetInhibitorArcs().end();
			arc++){
		if(marking->NumberOfTokensInPlace(arc->lock()->InputPlace().GetIndex()) >= arc->lock()->GetWeight()){
			pair<int, int> p(-1, -1);
			return p;
		}
	}

	for(TAPN::TimedInputArc::WeakPtrVector::const_iterator arc = transition.GetPreset().begin(); arc != transition.GetPreset().end(); arc++){
		vector<Util::interval > intervals;
		int range;
		if(arc->lock()->Interval().GetUpperBound() == INT_MAX){
			range = INT_MAX;
		}else{
			range = arc->lock()->Interval().GetUpperBound()-arc->lock()->Interval().GetLowerBound();
		}
		int weight = arc->lock()->GetWeight();

		TokenList tokens = marking->GetTokenList(arc->lock()->InputPlace().GetIndex());
		if(tokens.size() == 0){
			pair<int, int> p(-1, -1);
			return p;
		}

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
				int low = arc->lock()->Interval().GetLowerBound() - tokens.at(i).getAge();
				int heigh = arc->lock()->Interval().GetUpperBound() - tokens.at(j).getAge();

				Util::interval element(low < 0 ? 0 : low,
						arc->lock()->Interval().GetUpperBound() == INT_MAX ? INT_MAX : heigh);
				Util::set_add(intervals, element);
			}
			numberOfTokensAvailable -= tokens.at(i).getCount();
		}

		start = Util::setIntersection(start, intervals);
	}

	// Transport arcs
	for(TAPN::TransportArc::WeakPtrVector::const_iterator arc = transition.GetTransportArcs().begin(); arc != transition.GetTransportArcs().end(); arc++){
		Util::interval arcGuard(arc->lock()->Interval().GetLowerBound(), arc->lock()->Interval().GetUpperBound());
		Util::interval invGuard(0, arc->lock()->Destination().GetInvariant().GetBound());

		Util::interval arcInterval = boost::numeric::intersect(arcGuard, invGuard);
		vector<Util::interval > intervals;
		int range;
		if(arcInterval.upper() == INT_MAX){
			range = INT_MAX;
		}else{
			range = arcInterval.upper()-arcInterval.upper();
		}
		int weight = arc->lock()->GetWeight();

		TokenList tokens = marking->GetTokenList(arc->lock()->Source().GetIndex());

		if(tokens.size() == 0){
			pair<int, int> p(-1, -1);
			return p;
		}

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
				Util::interval element(arcInterval.lower() - tokens.at(i).getAge(),
						arcInterval.upper() - tokens.at(j).getAge());
				Util::set_add(intervals, element);
			}
			numberOfTokensAvailable -= tokens.at(i).getCount();
		}

		start = Util::setIntersection(start, intervals);
	}

	int invariantPart = maxPossibleDelay(marking);

	vector<Util::interval > invEnd;
	Util::interval initialInv(0, invariantPart);
	invEnd.push_back(initialInv);
	start = Util::setIntersection(start, invEnd);

#if DEBUG
	std::cout << "Intervals in start: " << start.size() << std::endl;
#endif

	if(start.empty()){
		pair<int, int> p(-1,-1);
		return p;
	}else{
		pair<int, int> p(start.front().lower(), start.back().upper());
		return p;
	}
}

int TimeDartLiveness::calculateEnd(const TimedTransition& transition, NonStrictMarking* marking){

	int part1 = 0;

	PlaceList placeList = marking->GetPlaceList();

	for(PlaceList::const_iterator place_iter = placeList.begin(); place_iter != placeList.end(); place_iter++){
		//The smallest age is the first as the tokens is sorted
		int maxDelay = place_iter->place->GetMaxConstant() - place_iter->tokens.at(0).getAge();
		if(maxDelay > part1){
			part1 = maxDelay;
		}
	}

	//should be maxconstant + 1
	part1++;

	int part2 = INT_MAX;

	for(PlaceList::const_iterator iter = marking->GetPlaceList().begin(); iter != marking->GetPlaceList().end(); iter++){
		if(iter->place->GetInvariant().GetBound() != std::numeric_limits<int>::max() && iter->place->GetInvariant().GetBound()-iter->tokens.back().getAge() < part2){
			part2 = iter->place->GetInvariant().GetBound()-iter->tokens.back().getAge();
		}
	}

	return min(part1, part2);
}

int TimeDartLiveness::calculateStop(const TimedTransition& transition, NonStrictMarking* marking){
	int MC = -1;

	unsigned int i = 0;
	for(PlaceList::const_iterator iter = marking->GetPlaceList().begin(); iter != marking->GetPlaceList().end(); iter++){
		if(i < transition.GetPreset().size() && iter->place->GetIndex() == transition.GetPreset().at(i).lock()->InputPlace().GetIndex()){
			if(transition.GetPreset().at(i).lock()->GetWeight() < iter->NumberOfTokens()){
				// TODO it should be the youngest NumberOfTokens-GetWeight'e token for optimization
				MC = max(MC, iter->place->GetMaxConstant() - iter->tokens.front().getAge());
			}
			i++;
			continue;
		}
		MC = max(MC, iter->place->GetMaxConstant() - iter->tokens.front().getAge());
	}

	return MC+1;
}

void TimeDartLiveness::printStats(){
	std::cout << "  discovered markings:\t" << pwList.discoveredMarkings << std::endl;
	std::cout << "  explored markings:\t" << exploredMarkings << std::endl;
	std::cout << "  stored markings:\t" << pwList.Size() << std::endl;
}

TimeDartLiveness::~TimeDartLiveness() {
}

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
