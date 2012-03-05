/*
 * DiscreteVerification.cpp
 *
 *  Created on: 23 Feb 2012
 *      Author: jakob
 */

#include <iostream>
#include "DiscreteVerification.hpp"
#include "../Core/TAPN/TAPN.hpp"
#include "boost/smart_ptr.hpp"
#include "../Core/QueryParser/AST.hpp"
#include "NonStrictMarking.hpp"


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
		std::cout << "The supplied network is contains strict intervals." << std::endl;
		return 1;
	}

	NonStrictMarking* initialMarking = new NonStrictMarking(initialPlacement);

	std::cout << initialMarking << std::endl;

	if(initialMarking->size() > options.GetKBound())
	{
		std::cout << "The specified k-bound is less than the number of tokens in the initial markings.";
		return 1;
	}



	return 0;
}

}

}
