/*
 * DiscreteVerification.cpp
 *
 *  Created on: 23 Feb 2012
 *      Author: jakob
 */

#include "DiscreteVerification.hpp"

namespace VerifyTAPN {

namespace DiscreteVerification {

DiscreteVerification::DiscreteVerification() {
	// TODO Auto-generated constructor stub

}

DiscreteVerification::~DiscreteVerification() {
	// TODO Auto-generated destructor stub
}

int DiscreteVerification::run(boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn, std::vector<int> initialPlacement, AST::Query* query, VerificationOptions& options){
	if(!(*tapn).IsNonStrict()){
		std::cout << "The supplied net contains strict intervals." << std::endl;
		return -1;
	}

	NonStrictMarking* initialMarking = new NonStrictMarking(*tapn, initialPlacement);

	std::cout << "MC: " << tapn->MaxConstant() << std::endl;

	if(initialMarking->size() > options.GetKBound())
	{
		std::cout << "The specified k-bound is less than the number of tokens in the initial markings.";
		return 1;
	}

	std::cout << options << std::endl;
	Verification* verifier = NULL;

	WaitingList<NonStrictMarking>* strategy = NULL;
	// Select verification method
	if(options.GetVerificationType() == DISCRETE){
		WaitingList<NonStrictMarking>* strategy = GetWaitingList<NonStrictMarking>(query, options);

		if(query->GetQuantifier() == EG || query->GetQuantifier() == AF){
			verifier = new LivenessSearch(tapn, *initialMarking, query, options, strategy);
		}else if(query->GetQuantifier() == EF || query->GetQuantifier() == AG){
			verifier = new ReachabilitySearch(tapn, *initialMarking, query, options, strategy);
		}
	}else if(options.GetVerificationType() == TIMEDART){
		WaitingList<TimeDart>* strategy = GetWaitingList<TimeDart>(query, options);

		if(query->GetQuantifier() == EG || query->GetQuantifier() == AF){
			std::cout << "Liveness not implemented for time darts" << std::endl;
			exit(1);
		}else if(query->GetQuantifier() == EF || query->GetQuantifier() == AG){

			verifier = new TimeDartReachabilitySearch(tapn, *initialMarking, query, options, strategy);
		}
	}

	bool result = (query->GetQuantifier() == AG || query->GetQuantifier() == AF)? !verifier->Verify() : verifier->Verify();

	verifier->printStats();
	verifier->PrintTransitionStatistics();

	std::cout << "Query is " << (result ? "satisfied" : "NOT satisfied") << "." << std::endl;
	std::cout << "Max number of tokens found in any reachable marking: ";
	if(verifier->MaxUsedTokens() > options.GetKBound())
		std::cout << ">" << options.GetKBound() << std::endl;
	else
		std::cout << verifier->MaxUsedTokens() << std::endl;

	if(options.GetTrace() == SOME){
		std::stack<NonStrictMarking*> printStack;
		if((query->GetQuantifier() == EF && result) || (query->GetQuantifier() == AG && !result)) {
			GenerateTraceStack(verifier->GetLastMarking(), &printStack);
			if(options.XmlTrace()){
				PrintXMLTrace(result, verifier->GetLastMarking(), printStack, query->GetQuantifier());
			} else {
				PrintHumanTrace(result, verifier->GetLastMarking(), printStack, query->GetQuantifier());
			}
		} else if((query->GetQuantifier() == EG && result) || (query->GetQuantifier() == AF && !result)) {
			NonStrictMarking* m = verifier->trace.top();
			GenerateTraceStack(m, &printStack, &verifier->trace);
			if(options.XmlTrace()){
				PrintXMLTrace(result, m, printStack, query->GetQuantifier());
			} else {
				PrintHumanTrace(result, m, printStack, query->GetQuantifier());
			}
		} else {
			std::cout << "A trace could not be generated due to the query result" << std::endl;
		}
	}

	delete strategy;

	return 0;
}

void DiscreteVerification::PrintHumanTrace(bool result, NonStrictMarking* m, std::stack<NonStrictMarking*>& stack, AST::Quantifier query) {
	std::cout << "Trace: " << std::endl;
	bool isFirst = true;
	bool foundLoop = false;

	while(!stack.empty()){
		if(isFirst) {
			isFirst = false;
		} else {
			if(stack.top()->GetGeneratedBy()){
				std::cout << "\tTransistion: " << stack.top()->GetGeneratedBy()->GetName() << std::endl;
			}else{
				int i = 1;
				NonStrictMarking* old = stack.top();
				stack.pop();
				while(!stack.empty() && stack.top()->GetGeneratedBy() == NULL) {
					old = stack.top();
					stack.pop();
					i++;
				}

				if(stack.empty() && old->children > 0){
					std::cout << "\tDelay: Forever"  << std::endl;
					return;
				}

				std::cout << "\tDelay: " << i << std::endl;
				stack.push(old);
			}
		}

		if((query == EG || query == AF)
				&& (stack.size() > 1 && stack.top()->equals(*m))
				&& (m->GetGeneratedBy() || stack.top()->parent)){
			std::cout << "\t* ";
			foundLoop = true;
		} else {
			std::cout << "\t";
		}

		//Print marking
		std::cout << "Marking: ";
		for(PlaceList::const_iterator iter = stack.top()->places.begin(); iter != stack.top()->places.end(); iter++){
			for(TokenList::const_iterator titer = iter->tokens.begin(); titer != iter->tokens.end(); titer++){
				for(int i = 0; i < titer->getCount(); i++) {
					std::cout << "(" << iter->place->GetName() << "," << titer->getAge() << ") ";
				}
			}
		}

		std::cout << std::endl;
		stack.pop();
	}

	//Trace ended, goto * or deadlock
	if(query == EG || query == AF){
		if(foundLoop){
			std::cout << "\tgoto *" << std::endl;
		} else {
			if(m->children > 0){
				std::cout << "\tDeadlock" << std::endl;
			}else{
				for(PlaceList::const_iterator iter = m->places.begin(); iter != m->places.end(); iter++){
					if(iter->place->GetInvariant().GetBound() != std::numeric_limits<int>::max()){
						//Invariant, deadlock
						std::cout << "\tDeadlock" << std::endl;
						return;
					}
				}
				std::cout << "\tDelay: Forever"  << std::endl;
			}
		}

	}
}

void DiscreteVerification::PrintXMLTrace(bool result, NonStrictMarking* m, std::stack<NonStrictMarking*>& stack, AST::Quantifier query) {
	using namespace rapidxml;
	std::cerr << "Trace: " << std::endl;
	bool isFirst = true;
	bool foundLoop = false;
	bool delayedForever = false;
	NonStrictMarking* old;

	xml_document<> doc;
	xml_node<>* root = doc.allocate_node(node_element, "trace");
	doc.append_node(root);

	while(!stack.empty()){
		if(isFirst) {
			isFirst = false;
		} else {
			if(stack.top()->GetGeneratedBy()){
				root->append_node(CreateTransitionNode(old, stack.top(), doc));
			} else{
				int i = 1;
				old = stack.top();
				stack.pop();
				while(!stack.empty() && stack.top()->GetGeneratedBy() == NULL) {
					old = stack.top();
					stack.pop();
					i++;
				}

				if(stack.empty() && old->children > 0){
					xml_node<>* node = doc.allocate_node(node_element, "delay", doc.allocate_string("forever"));
					root->append_node(node);
					delayedForever = true;
					break;
				}
				xml_node<>* node = doc.allocate_node(node_element, "delay", doc.allocate_string(ToString(i).c_str()));
				root->append_node(node);
				stack.push(old);
			}
		}

		if((query == EG || query == AF)
				&& (stack.size() > 1 && stack.top()->equals(*m))
				&& (m->GetGeneratedBy() || stack.top()->parent)){
			root->append_node(doc.allocate_node(node_element, "loop"));
			foundLoop = true;
		}
		old = stack.top();
		stack.pop();
	}

	//Trace ended, goto * or deadlock
	if(query == EG || query == AF){
		if(!foundLoop && !delayedForever) {
			if(m->children > 0){
				root->append_node(doc.allocate_node(node_element, "deadlock"));
			}else{
				// By default delay forever
				xml_node<>* node = doc.allocate_node(node_element, "delay", doc.allocate_string("forever"));
				for(PlaceList::const_iterator iter = m->places.begin(); iter != m->places.end(); iter++){
					if(iter->place->GetInvariant().GetBound() != std::numeric_limits<int>::max()){
						//Invariant, deadlock instead of delay forever
						node = doc.allocate_node(node_element, "deadlock");
						break;
					}
				}
				root->append_node(node);
			}
		}
	}

	std::cerr << doc;
}

rapidxml::xml_node<>* DiscreteVerification::CreateTransitionNode(NonStrictMarking* old, NonStrictMarking* current, rapidxml::xml_document<>& doc){
	using namespace rapidxml;
	xml_node<>* transitionNode = doc.allocate_node(node_element, "transition");
	xml_attribute<>* id = doc.allocate_attribute("id", current->generatedBy->GetId().c_str());
	transitionNode->append_attribute(id);

	for(TAPN::TimedInputArc::WeakPtrVector::const_iterator arc_iter = current->generatedBy->GetPreset().begin(); arc_iter != current->generatedBy->GetPreset().end(); arc_iter++){
		createTransitionSubNodes(old, current, doc, transitionNode, arc_iter->lock()->InputPlace(), arc_iter->lock()->Interval(), arc_iter->lock()->GetWeight());
	}

	for(TAPN::TransportArc::WeakPtrVector::const_iterator arc_iter = current->generatedBy->GetTransportArcs().begin(); arc_iter != current->generatedBy->GetTransportArcs().end(); arc_iter++){
		createTransitionSubNodes(old, current, doc, transitionNode, arc_iter->lock()->Source(), arc_iter->lock()->Interval(), arc_iter->lock()->GetWeight());
	}

	return transitionNode;
}

void DiscreteVerification::createTransitionSubNodes(NonStrictMarking* old, NonStrictMarking* current, rapidxml::xml_document<>& doc, rapidxml::xml_node<>* transitionNode, const TAPN::TimedPlace& place, const TAPN::TimeInterval& interval, const int weight){
	TokenList current_tokens = current->GetTokenList(place.GetIndex());
	TokenList old_tokens = old->GetTokenList(place.GetIndex());
	int tokensFound = 0;

	TokenList::const_iterator n_iter = current_tokens.begin();
	TokenList::const_iterator o_iter = old_tokens.begin();
	while(n_iter != current_tokens.end() && o_iter != old_tokens.end()){
		if(n_iter->getAge() == o_iter->getAge()){
			for(int i = 0; i < o_iter->getCount() - n_iter->getCount(); i++){
				transitionNode->append_node(createTokenNode(doc, place, *n_iter));
				tokensFound++;
			}
			n_iter++;
			o_iter++;
		} else {
			if(n_iter->getAge() > o_iter->getAge()){
				transitionNode->append_node(createTokenNode(doc, place, *o_iter));
				tokensFound++;
				o_iter++;
			} else {
				n_iter++;
			}
		}
	}

	for(TokenList::const_iterator iter = o_iter; iter != old_tokens.end(); iter++){
		for(int i = 0; i < iter->getCount(); i++){
			transitionNode->append_node(createTokenNode(doc, place, *iter));
			tokensFound++;
		}
	}

	for(TokenList::const_iterator iter = old_tokens.begin(); iter != old_tokens.end() && tokensFound < weight; iter++){
		if(iter->getAge() >= interval.GetLowerBound()){
			for(int i = 0; i < iter->getCount() && tokensFound < weight; i++){
				transitionNode->append_node(createTokenNode(doc, place, *iter));
				tokensFound++;
			}
		}
	}
}

rapidxml::xml_node<>* DiscreteVerification::createTokenNode(rapidxml::xml_document<>& doc, const TAPN::TimedPlace& place, const Token& token){
	using namespace rapidxml;
	xml_node<>* tokenNode = doc.allocate_node(node_element, "token");
	xml_attribute<>* placeAttribute = doc.allocate_attribute("place", doc.allocate_string(place.GetName().c_str()));
	tokenNode->append_attribute(placeAttribute);
	xml_attribute<>* ageAttribute = doc.allocate_attribute("age", doc.allocate_string( ToString(token.getAge()).c_str()));
	tokenNode->append_attribute(ageAttribute);
	if(place.GetMaxConstant() < token.getAge()){
		xml_attribute<>* gtAttribute = doc.allocate_attribute("greaterThanOrEqual", doc.allocate_string("true"));
		tokenNode->append_attribute(gtAttribute);
	} else {
		xml_attribute<>* gtAttribute = doc.allocate_attribute("greaterThanOrEqual", doc.allocate_string("false"));
		tokenNode->append_attribute(gtAttribute);
	}

	return tokenNode;
}

void DiscreteVerification::GenerateTraceStack(NonStrictMarking* m, std::stack<NonStrictMarking*>* result ,std::stack<NonStrictMarking*>* liveness) {
	if(liveness == NULL) {
		NonStrictMarking* next = m;
		do{
			result->push(next);
		} while((next=next->parent) != NULL);
	} else {
		do{
			result->push(liveness->top());
			liveness->pop();
		} while(!(liveness->empty()));
	}
}

}

}
