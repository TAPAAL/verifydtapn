/*
 * NonStrictSearch.hpp
 *
 *  Created on: 26/04/2012
 *      Author: MathiasGS
 */

#ifndef TIMEDARTLIVENESS_HPP_
#define TIMEDARTLIVENESS_HPP_

#include "../DataStructures/TimeDart.hpp"
#include "../DataStructures/TimeDartLivenessPWList.hpp"
#include "boost/smart_ptr.hpp"
#include "boost/numeric/interval.hpp"
#include "../../Core/TAPN/TAPN.hpp"
#include "../../Core/QueryParser/AST.hpp"
#include "../../Core/VerificationOptions.hpp"
#include "../QueryVisitor.hpp"
#include "boost/any.hpp"
#include "../DataStructures/NonStrictMarking.hpp"
#include <stack>
#include "TimeDartVerification.hpp"
#include "../DataStructures/TimeDart.hpp"
#include "../Util/IntervalOps.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

class TimeDartLiveness : public TimeDartVerification{
public:
	TimeDartLiveness(boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<WaitingDart>* waiting_list);
	virtual ~TimeDartLiveness();
	bool Verify();
	inline unsigned int MaxUsedTokens(){ return pwList.maxNumTokensInAnyMarking; };

protected:
	bool addToPW(NonStrictMarking* marking, NonStrictMarking* parent, int start, int end);
	bool canDelayForever(NonStrictMarking* marking);

protected:
	int validChildren;
	TimeDartLivenessPWList pwList;
public:
	void printStats();
	void GetTrace();
private:
	TraceList* lastMarking;
	stack< TraceDart* > trace;
};

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* NONSTRICTSEARCH_HPP_ */
