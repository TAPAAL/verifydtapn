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
#if DEBUG
	std::cout << "Places: " << std::endl;
	for(TAPN::TimedPlace::Vector::const_iterator iter = tapn.get()->GetPlaces().begin(); iter != tapn.get()->GetPlaces().end(); iter++){
		std::cout << "Place " << iter->get()->GetIndex() << " has category " << iter->get()->GetType() << std::endl;
	}
#endif

	if(initialMarking->size() > options.GetKBound())
	{
		std::cout << "The specified k-bound is less than the number of tokens in the initial markings.";
		return 1;
	}

	std::cout << options << std::endl;
	Verification<NonStrictMarking>* verifier = NULL;

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
		if(query->GetQuantifier() == EG || query->GetQuantifier() == AF){
			WaitingList<WaitingDart>* strategy = GetWaitingList<WaitingDart>(query, options);
			verifier = new TimeDartLiveness(tapn, *initialMarking, query, options, strategy);
		}else if(query->GetQuantifier() == EF || query->GetQuantifier() == AG){
			WaitingList<TimeDart>* strategy = GetWaitingList<TimeDart>(query, options);
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
		if((query->GetQuantifier() == EF && result) || (query->GetQuantifier() == AG && !result) || (query->GetQuantifier() == EG && result) || (query->GetQuantifier() == AF && !result)) {
			verifier->GetTrace();
		}else {
			std::cout << "A trace could not be generated due to the query result" << std::endl;
		}
	}

	delete strategy;

	return 0;
}

}

}
