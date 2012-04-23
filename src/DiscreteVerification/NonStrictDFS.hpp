/*
 * NonStrictDFS.hpp
 *
 *  Created on: 05/03/2012
 *      Author: MathiasGS
 */

#ifndef NONSTRICTDFS_HPP_
#define NONSTRICTDFS_HPP_
#define DEBUG 0

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

#include "QueryVisitor.hpp"
#include "boost/any.hpp"

#include <stack>

namespace VerifyTAPN {

namespace DiscreteVerification {

class NonStrictDFS {
public:
	NonStrictDFS(boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options);
	virtual ~NonStrictDFS();
	bool Verify();

private:
	vector<NonStrictMarking> getPossibleNextMarkings(NonStrictMarking& marking);
	bool addToPW(NonStrictMarking* marking);
	bool isKBound(NonStrictMarking& marking);
	bool isDelayPossible(NonStrictMarking& marking);
	NonStrictMarking* cut(NonStrictMarking& marking);
private:
	int validChildren;
	stack<NonStrictMarking> trace;
	PWList pwList;
	boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn;
	NonStrictMarking& initialMarking;
	AST::Query* query;
	VerificationOptions options;
	SuccessorGenerator successorGenerator;
};

}
}
#endif /* NONSTRICTDFS_HPP_ */
