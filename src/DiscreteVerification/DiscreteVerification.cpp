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
		while(!strategy->trace.empty()){
			std::cout << *strategy->trace.top() << std::endl;
			strategy->trace.pop();
		}
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

	PrintTraceIfAny(result, strategy->GetLastMarking());

	delete strategy;

	return 0;
}

void DiscreteVerification::PrintTraceIfAny(bool result, NonStrictMarking* m) {
	if(result){
		std::stack < NonStrictMarking* > stack;
		std::cout << "Trace: " << std::endl;
		NonStrictMarking* next = m;
		bool isFirst = true;
		do{
			stack.push(next);
		} while((next=next->parent) != NULL);

		while(!stack.empty()){
			if(isFirst) {
				isFirst = false;
			} else {
				if(stack.top()->GetGeneratedBy()){
					std::cout << "Transistion: " << stack.top()->GetGeneratedBy()->GetName() << std::endl;
				} else{
					std::cout << "Delay: 1" << std::endl;
				}
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
			std::cout << "Stack before: " << stack.size() << std::endl;
			stack.pop();
			std::cout << "Stack after: " << stack.size() << std::endl;
		}
	}
}

}

}
