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
: pwList(waiting_list), tapn(tapn), initialMarking(initialMarking), query(query), options(options), successorGenerator( *tapn.get() ), allwaysEnabled(), exploredMarkings(0), trace(10000){

	//Find the transitions which don't have input arcs
	for(TimedTransition::Vector::const_iterator iter = tapn->GetTransitions().begin(); iter != tapn->GetTransitions().end(); iter++){
		if((*iter)->GetPreset().size() + (*iter)->GetTransportArcs().size() == 0){
			allwaysEnabled.push_back(iter->get());
		}
	}
}

bool TimeDartReachabilitySearch::Verify(){
	if(options.GetTrace() == SOME){
		initialMarking.SetGeneratedBy(NULL);
		addToTrace(&initialMarking, NULL, 0);
	}

	if(addToPW(&initialMarking)){
		return true;
		if(options.GetTrace() == SOME){
			lastMarking = &initialMarking;
		}
	}

	//Main loop
	while(pwList.HasWaitingStates()){
		TimeDart& dart = *pwList.GetNextUnexplored();
		exploredMarkings++;

		int passed = dart.getPassed();
		dart.setPassed(dart.getWaiting());
		tapn->GetTransitions();
		for(TimedTransition::Vector::const_iterator transition_iter = tapn->GetTransitions().begin();
				transition_iter != tapn->GetTransitions().end(); transition_iter++){
			TimedTransition& transition = **transition_iter;
			pair<int,int> calculatedStart = calculateStart(transition, dart.getBase());
			if(calculatedStart.first == -1){	// Transition cannot be enabled in marking
				continue;
			}
			int start = max(dart.getWaiting(), calculatedStart.first);
			int end = min(passed-1, calculatedStart.second);
			if(start <= end){

				if(transition.hasUntimedPostset()){
					NonStrictMarking Mpp(*dart.getBase());
					Mpp.incrementAge(start);
					vector<NonStrictMarking*> next = getPossibleNextMarkings(Mpp, transition);
					for(vector<NonStrictMarking*>::iterator it = next.begin(); it != next.end(); it++){
						if(addToPW(*it)){
							if(options.GetTrace() == SOME){
								lastMarking = *it;
								(*it)->SetGeneratedBy(&transition);
								addToTrace(*it, dart.getBase(), start);
							}
							return true;
						}
						if(options.GetTrace() == SOME){
							(*it)->SetGeneratedBy(&transition);
							addToTrace(*it, dart.getBase(), start);
						}
					}
				}else{
					int stop = min(max(start, calculateStop(transition, dart.getBase())), end);
					for(int n = start; n <= stop; n++){
						NonStrictMarking Mpp(*dart.getBase());
						Mpp.incrementAge(n);

						vector<NonStrictMarking*> next = getPossibleNextMarkings(Mpp, transition);
						for(vector<NonStrictMarking*>::iterator it = next.begin(); it != next.end(); it++){
							if(addToPW(*it)){
								if(options.GetTrace() == SOME){
									lastMarking = *it;
									(*it)->SetGeneratedBy(&transition);
									addToTrace(*it, dart.getBase(), n);
								}
								return true;
							}
							if(options.GetTrace() == SOME){
								(*it)->SetGeneratedBy(&transition);
								addToTrace(*it, dart.getBase(), n);
							}
						}
					}
				}
			}
		}
	}

	return false;
}

void TimeDartReachabilitySearch::GetTrace(){
	stack<TraceList*> traceStack;

	TraceList* current = new TraceList(lastMarking, 0);
	while(current->first != NULL){
		traceStack.push(current);
		current = &trace[current->first];
	}

	PrintXMLTrace(lastMarking, traceStack);
}

xml_node<>* TimeDartReachabilitySearch::generateTransitionNode(NonStrictMarking* from, NonStrictMarking* to, xml_document<> doc){
	xml_node<>* node = doc.allocate_node(node_element, "transition", doc.allocate_string(ToString(to->GetGeneratedBy()->GetId()).c_str()));
	return node;
}

xml_node<>* TimeDartReachabilitySearch::generateDelayNode(int delay, xml_document<> doc){
	xml_node<>* node = doc.allocate_node(node_element, "delay", doc.allocate_string(ToString(delay).c_str()));
	return node;
}

void TimeDartReachabilitySearch::PrintXMLTrace(NonStrictMarking* m, std::stack<TraceList*>& stack) {
	using namespace rapidxml;
	std::cerr << "Trace: " << std::endl;
	TraceList* old = NULL;

	xml_document<> doc;
	xml_node<>* root = doc.allocate_node(node_element, "trace");
	doc.append_node(root);

	while(!stack.empty()){

		if(stack.top()->first->generatedBy != NULL){
			root->append_node(CreateTransitionNode(old->first, stack.top()->first, doc));
		}
		if(stack.top()->second > 0){
			xml_node<>* node = doc.allocate_node(node_element, "delay", doc.allocate_string(ToString(stack.top()->second).c_str()));
			root->append_node(node);
			stack.top()->first->incrementAge(stack.top()->second);

		}

		old = stack.top();
		stack.pop();
	}

	std::cerr << doc;
}



void TimeDartReachabilitySearch::addToTrace(NonStrictMarking* marking, NonStrictMarking* parent, int d){
	TraceList& m = trace[marking];
	m.first = parent;
	m.second = max(d, m.second);
}

vector<NonStrictMarking*> TimeDartReachabilitySearch::getPossibleNextMarkings(NonStrictMarking& marking, const TimedTransition& transition){
	return successorGenerator.generateSuccessors(marking, transition);
}

bool TimeDartReachabilitySearch::addToPW(NonStrictMarking* marking){
	marking->cut();

	unsigned int size = marking->size();

	pwList.SetMaxNumTokensIfGreater(size);

	if(size > options.GetKBound()) {
		return false;
	}

	if(pwList.Add(tapn.get(), marking)){
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

pair<int,int> TimeDartReachabilitySearch::calculateStart(const TimedTransition& transition, NonStrictMarking* marking){
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

	int invariantPart = INT_MAX;

	for(PlaceList::const_iterator iter = marking->GetPlaceList().begin(); iter != marking->GetPlaceList().end(); iter++){
		if(iter->place->GetInvariant().GetBound() != std::numeric_limits<int>::max() && iter->place->GetInvariant().GetBound()-iter->tokens.back().getAge() < invariantPart){
			invariantPart = iter->place->GetInvariant().GetBound()-iter->tokens.back().getAge();
		}
	}

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

int TimeDartReachabilitySearch::calculateEnd(const TimedTransition& transition, NonStrictMarking* marking){

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

int TimeDartReachabilitySearch::calculateStop(const TimedTransition& transition, NonStrictMarking* marking){
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

void TimeDartReachabilitySearch::printStats(){
	std::cout << "  discovered markings:\t" << pwList.discoveredMarkings << std::endl;
	std::cout << "  explored markings:\t" << exploredMarkings << std::endl;
	std::cout << "  stored markings:\t" << pwList.Size() << std::endl;
}

TimeDartReachabilitySearch::~TimeDartReachabilitySearch() {
}

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
