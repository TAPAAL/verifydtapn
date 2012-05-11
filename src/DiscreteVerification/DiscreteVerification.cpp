/*
 * DiscreteVerification.cpp
 *
 *  Created on: 23 Feb 2012
 *      Author: jakob
 */

#include "DiscreteVerification.hpp"
#include "NonStrictDFS.hpp"
#include "NonStrictBFS.hpp"
#include "NonStrictHeuristic.hpp"
#include "NonStrictRandom.hpp"
#include "NonStrictSearch.hpp"
#include "NonStrictDFSHeuristic.hpp"
#include "NonStrictDFSRandom.hpp"
#include "../Core/TAPNParser/util.hpp"

namespace VerifyTAPN {

namespace DiscreteVerification {

DiscreteVerification::DiscreteVerification() {
	// TODO Auto-generated constructor stub

}

DiscreteVerification::~DiscreteVerification() {
	// TODO Auto-generated destructor stub
}

int DiscreteVerification::run(boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn, std::vector<int> initialPlacement, AST::Query* query, VerificationOptions options){
	if(!(*tapn).IsNonStrict()){
		std::cout << "The supplied network contains strict intervals." << std::endl;
		return -1;
	}

	NonStrictMarking* initialMarking = new NonStrictMarking(*tapn, initialPlacement);

	std::cout << "MC: " << tapn->MaxConstant() << std::endl;
	std::cout << "initialMarking: " << *initialMarking << std::endl;
	std::cout << "size: " << initialMarking->size() << std::endl;
	std::cout << "hash: " << boost::hash_value(initialMarking) << std::endl;

	if(initialMarking->size() > options.GetKBound())
	{
		std::cout << "The specified k-bound is less than the number of tokens in the initial markings.";
		return 1;
	}

	std::cout << options << std::endl;
	// Select search strategy
	NonStrictSearch* strategy;
	if(query->GetQuantifier() == EG || query->GetQuantifier() == AF){
		//Liveness query, force DFS
		switch(options.GetSearchType()){
		case DEPTHFIRST:
			strategy = new NonStrictDFS(tapn, *initialMarking, query, options);
			break;
		case RANDOM:
			strategy = new NonStrictDFSRandom(tapn, *initialMarking, query, options);
			break;
		case COVERMOST:
			strategy = new NonStrictDFSHeuristic(tapn, *initialMarking, query, options);
			break;
		default:
			strategy = new NonStrictDFSHeuristic(tapn, *initialMarking, query, options);
			break;
		}
	}else{
		switch(options.GetSearchType()){
		case DEPTHFIRST:
			strategy = new NonStrictDFS(tapn, *initialMarking, query, options);
			break;
		case COVERMOST:
			strategy = new NonStrictHeuristic(tapn, *initialMarking, query, options);
			break;
		case BREADTHFIRST:
			strategy = new NonStrictBFS(tapn, *initialMarking, query, options);
			break;
		case RANDOM:
			strategy = new NonStrictRandom(tapn, *initialMarking, query, options);
			break;
		}
	}

	bool result = (query->GetQuantifier() == AG || query->GetQuantifier() == AF)? !strategy->Verify() : strategy->Verify();

	/*if(query->GetQuantifier() == EG || query->GetQuantifier() == AF){
		std::cout << "Trace (length = "<< strategy->trace.size() <<"): " << std::endl;
		while(!strategy->trace.empty()){
			std::cout << *strategy->trace.top() << std::endl;
			strategy->trace.pop();
		}
	}*/

	strategy->printStats();

	//std::cout << strategy->GetStats() << std::endl;
	std::cout << "Query is " << (result ? "satisfied" : "NOT satisfied") << "." << std::endl;
	std::cout << "Max number of tokens found in any reachable marking: ";
	if(strategy->MaxUsedTokens() > options.GetKBound())
		std::cout << ">" << options.GetKBound() << std::endl;
	else
		std::cout << strategy->MaxUsedTokens() << std::endl;

	/*try{
		strategy->PrintTraceIfAny(result);
	}catch(const trace_exception& e){
		std::cout << "There was an error generating a trace. This is a bug. Please report this on launchpad and attach your TAPN model and this error message: ";
		std::cout << e.what() << std::endl;
		return 1;
	}*/

	if(options.GetTrace() == SOME){
		std::stack<NonStrictMarking*> printStack;
		if((query->GetQuantifier() == EF && result) || (query->GetQuantifier() == AG && !result)) {
			GenerateTraceStack(strategy->GetLastMarking(), &printStack);
			if(options.XmlTrace()){
				PrintXMLTrace(result, strategy->GetLastMarking(), printStack, query->GetQuantifier());
			} else {
				PrintHumanTrace(result, strategy->GetLastMarking(), printStack, query->GetQuantifier());
			}
		} else if((query->GetQuantifier() == EG && result) || (query->GetQuantifier() == AF && !result)) {
			NonStrictMarking* m = strategy->trace.top();
			GenerateTraceStack(m, &printStack, &strategy->trace);
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
			} else{
				int i = 1;
				NonStrictMarking* old = stack.top();
				stack.pop();
				while(!(stack.empty()) && stack.top()->GetGeneratedBy() == NULL && !old->equals(*m)) {
					old = stack.top();
					stack.pop();
					i++;
				}
				if((stack.empty() && old->children > 0) || (stack.size() == 1 && old->equals(*m) && stack.top()->generatedBy == NULL && stack.top()->children > 0)){
					std::cout << "\tDelay: Forever"  << std::endl;
					return;
				}
				std::cout << "\tDelay: " << i << std::endl;
				stack.push(old);
			}
		}

		if((query == EG || query == AF) && (stack.top()->equals(*m) && stack.size() > 1)){
			std::cout << "\t* ";
			foundLoop = true;
		} else {
			std::cout << "\t";
		}
		std::cout << "Marking: ";
		for(PlaceList::const_iterator iter = stack.top()->places.begin(); iter != stack.top()->places.end(); iter++){
			for(TokenList::const_iterator titer = iter->tokens.begin(); titer != iter->tokens.end(); titer++){
				for(int i = 0; i < titer->getCount(); i++) {
					std::cout << "(" << iter->place->GetName() << "," << titer->getAge() << ") ";
				}
			}
		}
		std::cout << std::endl;
		//std::cout << "Stack before: " << stack.size() << std::endl;
		stack.pop();
		//std::cout << "Stack after: " << stack.size() << std::endl;
	}

	if(query == EG || query == AF){
		if(foundLoop){
			std::cout << "\tgoto *" << std::endl;
		} else {
			std::cout << "\tDeadlock" << std::endl;
		}

	}
}

void DiscreteVerification::PrintXMLTrace(bool result, NonStrictMarking* m, std::stack<NonStrictMarking*>& stack, AST::Quantifier query) {
	using namespace rapidxml;
	std::cerr << "Trace: " << std::endl;
	bool isFirst = true;
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
				while(!(stack.empty()) && stack.top()->GetGeneratedBy() == NULL && !old->equals(*m)) {
					old = stack.top();
					stack.pop();
					i++;
				}
				if((stack.empty() && old->children > 0) || (stack.size() == 1 && old->equals(*m) && stack.top()->generatedBy == NULL && stack.top()->children > 0)){
					xml_node<>* node = doc.allocate_node(node_element, "delay", doc.allocate_string("forever"));
					root->append_node(node);
					break;
				}
				xml_node<>* node = doc.allocate_node(node_element, "delay", doc.allocate_string(ToString(i).c_str()));
				root->append_node(node);
				stack.push(old);
			}
		}

		if((query == EG || query == AF) && (stack.top()->equals(*m) && stack.size() > 1)){
			root->append_node(doc.allocate_node(node_element, "loop"));
		}
		old = stack.top();
		stack.pop();
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
#if DEBUG
	assert(tokensFound == weight);
#endif
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
