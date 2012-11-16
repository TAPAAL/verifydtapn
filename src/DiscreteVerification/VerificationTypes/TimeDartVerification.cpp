#include "TimeDartVerification.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

using namespace std;

void TimeDartVerification::PrintXMLTrace(TraceList* m, std::stack<TraceList*>& stack, Quantifier query) {
	std::cerr << "Trace: " << std::endl;
	TraceList* old = stack.top();
	stack.pop();
	bool foundLoop = false;
	bool delayedForever = false;

	xml_document<> doc;
	xml_node<>* root = doc.allocate_node(node_element, "trace");
	doc.append_node(root);

	while(!stack.empty()){
		if(old->first != NULL){
			root->append_node(CreateTransitionNode(old->first, stack.top()->first, doc));
		}

		if(stack.size() > 1 && stack.top()->second > 0){
			xml_node<>* node = doc.allocate_node(node_element, "delay", doc.allocate_string(ToString(stack.top()->second).c_str()));
			root->append_node(node);
			stack.top()->first->incrementAge(stack.top()->second);
		}


		if((query == AST::EG || query == AST::AF)
				&& (stack.size() > 1 && stack.top()->first->equals(*m->first))){
			root->append_node(doc.allocate_node(node_element, "loop"));
			foundLoop = true;
		}
		old = stack.top();
		stack.pop();
	}

	//Trace ended, goto * or deadlock
	if(query == AST::EG || query == AST::AF){
		if(!foundLoop && !delayedForever) {
			// By default delay forever
			xml_node<>* node = doc.allocate_node(node_element, "delay", doc.allocate_string("forever"));
			for(PlaceList::const_iterator iter = m->first->places.begin(); iter != m->first->places.end(); iter++){
				if(iter->place->GetInvariant().GetBound() != std::numeric_limits<int>::max()){
					//Invariant, deadlock instead of delay forever
					if(m->second > 0){
						xml_node<>* node = doc.allocate_node(node_element, "delay", doc.allocate_string(ToString(m->second).c_str()));
						root->append_node(node);
					}
					node = doc.allocate_node(node_element, "deadlock");
					break;
				}
			}
			root->append_node(node);
		}
	}

	std::cerr << doc;
}

pair<int,int> TimeDartVerification::calculateStart(const TAPN::TimedTransition& transition, NonStrictMarking* marking){
	vector<Util::interval > start;
	Util::interval initial(0, INT_MAX);
	start.push_back(initial);

	if(transition.NumberOfInputArcs() + transition.NumberOfTransportArcs() == 0){ //always enabled
		pair<int, int> p(0, INT_MAX);
		return p;
	}

	// Inhibitor arcs
	for(TAPN::InhibitorArc::WeakPtrVector::const_iterator arc = transition.GetInhibitorArcs().begin();
			arc != transition.GetInhibitorArcs().end();
			arc++){
		if(marking->NumberOfTokensInPlace(arc->lock()->InputPlace().GetIndex()) >= arc->lock()->GetWeight()){
			pair<int, int> p(-1, -1);
			return p;
		}
	}


	// Standard arcs
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
				for(j++; j < tokens.size() && numberOfTokensAvailable < weight; j++){
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
				for(j++; j < tokens.size() && numberOfTokensAvailable < weight; j++){
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

int TimeDartVerification::calculateStop(const TAPN::TimedTransition& transition, NonStrictMarking* marking){
	int MC = -1;

	unsigned int i = 0;
	for(PlaceList::const_iterator iter = marking->GetPlaceList().begin(); iter != marking->GetPlaceList().end(); iter++){
		if(i < transition.GetPreset().size() && iter->place->GetIndex() == transition.GetPreset().at(i).lock()->InputPlace().GetIndex()){
			if(transition.GetPreset().at(i).lock()->GetWeight() < iter->NumberOfTokens()){
				MC = max(MC, iter->place->GetMaxConstant() - iter->tokens.front().getAge());
			}
			i++;
			continue;
		}
		MC = max(MC, iter->place->GetMaxConstant() - iter->tokens.front().getAge());
	}

	return MC+1;
}

int TimeDartVerification::maxPossibleDelay(NonStrictMarking* marking){
	int invariantPart = INT_MAX;

	for(PlaceList::const_iterator iter = marking->GetPlaceList().begin(); iter != marking->GetPlaceList().end(); iter++){
		if(iter->place->GetInvariant().GetBound() != std::numeric_limits<int>::max() && iter->place->GetInvariant().GetBound()-iter->tokens.back().getAge() < invariantPart){
			invariantPart = iter->place->GetInvariant().GetBound()-iter->tokens.back().getAge();
		}
	}

	return invariantPart;
}

vector<NonStrictMarking*> TimeDartVerification::getPossibleNextMarkings(NonStrictMarking& marking, const TAPN::TimedTransition& transition){
	return successorGenerator.generateSuccessors(marking, transition);
}


}
}
