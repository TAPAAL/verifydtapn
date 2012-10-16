/*
 * NonStrictSearch.hpp
 *
 *  Created on: 26/04/2012
 *      Author: MathiasGS
 */

#ifndef TIMEDARTLIVENESS_HPP_
#define TIMEDARTLIVENESS_HPP_

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
#include "../TimeDartSuccessorGenerator.hpp"
#include "../QueryVisitor.hpp"
#include "boost/any.hpp"
#include "../DataStructures/NonStrictMarking.hpp"
#include <stack>
#include "Verification.hpp"
#include "../DataStructures/TimeDart.hpp"
#include "../Util/IntervalOps.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

struct TraceDart{
	TimeDart* dart;
	int start;
	int end;
};

class TimeDartLiveness : public Verification{
public:
	TimeDartLiveness(boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<TimeDart>* waiting_list);
	virtual ~TimeDartLiveness();
	bool Verify();
	NonStrictMarking* GetLastMarking() { return lastMarking; }
	inline unsigned int MaxUsedTokens(){ return pwList.maxNumTokensInAnyMarking; };
	void PrintTransitionStatistics() const { successorGenerator.PrintTransitionStatistics(std::cout); }

protected:
	vector<NonStrictMarking*> getPossibleNextMarkings(NonStrictMarking& marking, const TimedTransition& transition);
	bool addToPW(NonStrictMarking* marking);
	bool isDelayPossible(NonStrictMarking& marking);
	pair<int,int> calculateStart(const TimedTransition& transition, NonStrictMarking* marking);
	int calculateEnd(const TimedTransition& transition, NonStrictMarking* marking);
	int calculateStop(const TimedTransition& transition, NonStrictMarking* marking);

protected:
	int validChildren;
	TimeDartPWList pwList;
	boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn;
	NonStrictMarking& initialMarking;
	AST::Query* query;
	VerificationOptions options;
	TimeDartSuccessorGenerator successorGenerator;
	vector<const TAPN::TimedTransition*> allwaysEnabled;
	int exploredMarkings;
public:
	void printStats();
	void GetTrace(){
		std::cout << "Trace not yet implemented" << std::endl;
	}
private:
	NonStrictMarking* lastMarking;
	stack< TraceDart > trace;
};

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* NONSTRICTSEARCH_HPP_ */
