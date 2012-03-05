/*
 * DiscreteVerification.hpp
 *
 *  Created on: 23 Feb 2012
 *      Author: jakob
 */

#ifndef DISCRETEVERIFICATION_HPP_
#define DISCRETEVERIFICATION_HPP_

#include "boost/smart_ptr.hpp"
#include "../Core/TAPN/TAPN.hpp"
#include "../Core/QueryParser/AST.hpp"
#include "../Core/VerificationOptions.hpp"

namespace VerifyTAPN {

namespace DiscreteVerification {

class DiscreteVerification {
public:
	DiscreteVerification();
	virtual ~DiscreteVerification();
	static int run(boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn, std::vector<int> initialPlacement, AST::Query* query, VerificationOptions options);
};

}

}

#endif /* DISCRETEVERIFICATION_HPP_ */
