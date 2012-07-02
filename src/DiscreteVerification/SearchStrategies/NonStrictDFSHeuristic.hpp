/*
 * NonStrictDFS.hpp
 *
 *  Created on: 05/03/2012
 *      Author: MathiasGS
 */

#ifndef NONSTRICTDFSHEURISTIC_HPP_
#define NONSTRICTDFSHEURISTIC_HPP_

#include "../DataStructures/PWList.hpp"
#include "boost/smart_ptr.hpp"
#include "../../Core/TAPN/TAPN.hpp"
#include "../../Core/QueryParser/AST.hpp"
#include "../../Core/VerificationOptions.hpp"

#include "../../Core/TAPN/TimedPlace.hpp"
#include "../../Core/TAPN/TimedTransition.hpp"
#include "../../Core/TAPN/TimedInputArc.hpp"
#include "../../Core/TAPN/TransportArc.hpp"
#include "../../Core/TAPN/InhibitorArc.hpp"
#include "../../Core/TAPN/OutputArc.hpp"

#include "../SuccessorGenerator.hpp"
#include "../NonStrictSearch.hpp"

#include "../QueryVisitor.hpp"
#include "boost/any.hpp"

#include <stack>

namespace VerifyTAPN {

namespace DiscreteVerification {

class NonStrictDFSHeuristic : public NonStrictSearch {
public:
	NonStrictDFSHeuristic(boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options)
	: NonStrictSearch(tapn, initialMarking, query, options, CreateWaitingList(query)){
		std::cout << "Using heuristic DFS strategy" << std::endl;
	};
	virtual ~NonStrictDFSHeuristic(){};

protected:
	virtual WaitingList* CreateWaitingList(AST::Query* query) const { return new HeuristicStackWaitingList(query); };
};

}
}
#endif /* NONSTRICTDFS_HPP_ */
