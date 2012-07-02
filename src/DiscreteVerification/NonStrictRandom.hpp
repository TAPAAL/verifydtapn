/*
 * NonStrictDFS.hpp
 *
 *  Created on: 05/03/2012
 *      Author: MathiasGS
 */

#ifndef NONSTRICTRANDOM_HPP_
#define NONSTRICTRANDOM_HPP_

#include "PWList.hpp"
#include "boost/smart_ptr.hpp"
#include "../Core/TAPN/TAPN.hpp"
#include "../Core/QueryParser/AST.hpp"
#include "../Core/VerificationOptions.hpp"

#include "../Core/TAPN/TimedPlace.hpp"
#include "../Core/TAPN/TimedTransition.hpp"
#include "../Core/TAPN/TimedInputArc.hpp"
#include "../Core/TAPN/TransportArc.hpp"
#include "../Core/TAPN/InhibitorArc.hpp"
#include "../Core/TAPN/OutputArc.hpp"

#include "SuccessorGenerator.hpp"
#include "NonStrictSearch.hpp"

#include "QueryVisitor.hpp"
#include "boost/any.hpp"

#include <stack>

namespace VerifyTAPN {

namespace DiscreteVerification {

class NonStrictRandom : public NonStrictSearch {
public:
	NonStrictRandom(boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options)
	: NonStrictSearch(tapn, initialMarking, query, options, CreateWaitingList(query)){
		std::cout << "Using random search strategy" << std::endl;
	};
	virtual ~NonStrictRandom(){};

protected:
	virtual WaitingList* CreateWaitingList(AST::Query* query) const { return new RandomWaitingList; };
};

}
}
#endif /* NONSTRICTDFS_HPP_ */
