/*
 * NonStrictSearch.hpp
 *
 *  Created on: 26/04/2012
 *      Author: MathiasGS
 */

#ifndef TIMEDARTREACHABILITYSEARCH_HPP_
#define TIMEDARTREACHABILITYSEARCH_HPP_

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
#include "../QueryVisitor.hpp"
#include "boost/any.hpp"
#include "../DataStructures/NonStrictMarking.hpp"
#include <stack>
#include "Verification.hpp"
#include "../DataStructures/TimeDart.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

class TimeDartReachabilitySearch : public Verification{
public:
	TimeDartReachabilitySearch(boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<TimeDart>* waiting_list);
	virtual ~TimeDartReachabilitySearch();
	bool Verify();
	NonStrictMarking* GetLastMarking() { return lastMarking; }
	inline unsigned int MaxUsedTokens(){ return pwList.maxNumTokensInAnyMarking; };
	void PrintTransitionStatistics() const { successorGenerator.PrintTransitionStatistics(std::cout); }

protected:
	vector<NonStrictMarking> getPossibleNextMarkings(NonStrictMarking& marking);
	bool addToPW(NonStrictMarking* marking, NonStrictMarking* parent);
	bool isDelayPossible(NonStrictMarking& marking);
	NonStrictMarking* cut(NonStrictMarking& marking);

protected:
	int validChildren;
	PWList pwList;
	boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn;
	NonStrictMarking& initialMarking;
	AST::Query* query;
	VerificationOptions options;
	SuccessorGenerator successorGenerator;
public:
	void printStats();
private:
	NonStrictMarking* lastMarking;
};

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* NONSTRICTSEARCH_HPP_ */
