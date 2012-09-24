/*
 * NonStrictSearch.hpp
 *
 *  Created on: 26/04/2012
 *      Author: MathiasGS
 */

#ifndef TIMEDARTREACHABILITYSEARCH_HPP_
#define TIMEDARTREACHABILITYSEARCH_HPP_

#include "../DataStructures/TimeDart.hpp"
#include "../DataStructures/TimeDartPWList.hpp"
#include "boost/smart_ptr.hpp"
#include "boost/numeric/interval.hpp"
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
#include "../Util/IntervalOps.hpp"

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
	bool addToPW(NonStrictMarking* marking, int w, int p);
	bool isDelayPossible(NonStrictMarking& marking);
	NonStrictMarking* cut(NonStrictMarking& marking);
	TimeDart makeDart(NonStrictMarking* marking, int w, int p);
	vector<TimedTransition> getTransitions(NonStrictMarking* marking);
	int calculateStart(const TimedTransition& transition, NonStrictMarking& marking);
	int calculateEnd(const TimedTransition& transition, NonStrictMarking& marking);

protected:
	int validChildren;
	TimeDartPWList pwList;
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
