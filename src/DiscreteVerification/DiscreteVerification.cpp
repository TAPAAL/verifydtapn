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


namespace VerifyTAPN {

namespace DiscreteVerification {

DiscreteVerification::DiscreteVerification() {
	// TODO Auto-generated constructor stub

}

DiscreteVerification::~DiscreteVerification() {
	// TODO Auto-generated destructor stub
}

int DiscreteVerification::run(boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn, std::vector<int> initialPlacement, AST::Query* query){
	if(!(*tapn).IsNonStrict()){
		std::cout << "The supplied network is contains strict intervals." << std::endl;
		return 1;
	}



	return 0;
}

}

}
