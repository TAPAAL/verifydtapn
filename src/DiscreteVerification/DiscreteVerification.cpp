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
		strategy = new NonStrictDFS(tapn, *initialMarking, query, options);
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

	if(query->GetQuantifier() == EG || query->GetQuantifier() == AF){
		std::cout << "Trace (length = "<< strategy->trace.size() <<"): " << std::endl;
		/*while(!strategy->trace.empty()){
			std::cout << *strategy->trace.top() << std::endl;
			strategy->trace.pop();
		}*/
	}

	//std::cout << strategy->GetStats() << std::endl;
	std::cout << "Query is " << (result ? "satisfied" : "NOT satisfied") << "." << std::endl;
	/*std::cout << "Max number of tokens found in any reachable marking: ";
	if(strategy->MaxUsedTokens() == options.GetKBound() + 1)
		std::cout << ">" << options.GetKBound() << std::endl;
	else
		std::cout << strategy->MaxUsedTokens() << std::endl;

	try{
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
			PrintTraceIfAny(result, strategy->GetLastMarking(), printStack, query->GetQuantifier());
		} else if((query->GetQuantifier() == EG && result) || (query->GetQuantifier() == AF && !result)) {
			NonStrictMarking* m = strategy->trace.top();
			GenerateTraceStack(m, &printStack, &strategy->trace);
			PrintTraceIfAny(result, m, printStack, query->GetQuantifier());
		} else {
			std::cout << "A trace could not be generated due to the query result" << std::endl;
		}
	}

	delete strategy;

	return 0;
}

void DiscreteVerification::PrintTraceIfAny(bool result, NonStrictMarking* m, std::stack<NonStrictMarking*>& stack, AST::Quantifier query) {
	std::cout << "Trace: " << std::endl;
	bool isFirst = true;

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
				if(stack.empty()){
					std::cout << "\tDelay: Forever"  << std::endl;
					return;
				}
				std::cout << "\tDelay: " << i << std::endl;
				stack.push(old);
			}
		}

		if((query == EG || query == AF) && (stack.top()->equals(*m) && stack.size() > 1)){
			std::cout << "\t* ";
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
