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
#include "../QueryVisitor.hpp"
#include "boost/any.hpp"
#include "../DataStructures/NonStrictMarking.hpp"
#include <stack>
#include "TimeDartVerification.hpp"
#include "../DataStructures/TimeDart.hpp"
#include "../Util/IntervalOps.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

using namespace rapidxml;

class TimeDartReachabilitySearch : public TimeDartVerification{
public:
	TimeDartReachabilitySearch(boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<TimeDart>* waiting_list);
	virtual ~TimeDartReachabilitySearch();
	bool Verify();
	NonStrictMarking* GetLastMarking() { return lastMarking; }
	inline unsigned int MaxUsedTokens(){ return pwList.maxNumTokensInAnyMarking; };

protected:
	bool addToPW(NonStrictMarking* marking);
	void addToTrace(NonStrictMarking* marking, NonStrictMarking* parent, int d);

protected:
	int validChildren;
	TimeDartPWList pwList;
	vector<const TAPN::TimedTransition*> allwaysEnabled;
public:
	void printStats();
	void GetTrace();
private:
	NonStrictMarking* lastMarking;
	google::sparse_hash_map<NonStrictMarking*, TraceList > trace;
};

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* NONSTRICTSEARCH_HPP_ */
