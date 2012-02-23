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

namespace VerifyTAPN {

namespace DiscreteVerification {

DiscreteVerification::DiscreteVerification() {
	// TODO Auto-generated constructor stub

}

DiscreteVerification::~DiscreteVerification() {
	// TODO Auto-generated destructor stub
}

int DiscreteVerification::run(boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn){
	if((*tapn).IsNonStrict()){
		std::cout << "Hello discrete world! " << std::endl;
	} else {
		std::cout << "NOooooo!! " << std::endl;
		return 1;
	}

	return 0;
}

}

}
