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
#include "../DataStructures/NonStrictMarkingBase.hpp"
#include <stack>
#include "TimeDartVerification.hpp"
#include "../DataStructures/TimeDart.hpp"
#include "../Util/IntervalOps.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

using namespace rapidxml;

class TimeDartReachabilitySearch : public TimeDartVerification{
public:
        TimeDartReachabilitySearch(boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn, NonStrictMarkingBase& initialMarking, AST::Query* query, VerificationOptions options) 
        :TimeDartVerification(tapn, options, query, initialMarking), trace(10000){};
        TimeDartReachabilitySearch(boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn, NonStrictMarkingBase& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<TimeDartBase>* waiting_list)
        :TimeDartVerification(tapn, options, query, initialMarking), trace(10000){
            pwList = new TimeDartPWHashMap(waiting_list, options.GetTrace());
        };
        virtual ~TimeDartReachabilitySearch();
	bool Verify();

	inline unsigned int MaxUsedTokens(){ return pwList->maxNumTokensInAnyMarking; };

protected:
	bool addToPW(NonStrictMarkingBase* marking, WaitingDart* parent, int upper);

protected:
	int validChildren;
	google::sparse_hash_map<NonStrictMarkingBase*, TraceList > trace;
        TimeDartPWBase* pwList;
	vector<const TAPN::TimedTransition*> allwaysEnabled;
        virtual inline void deleteBase(NonStrictMarkingBase* base){
           // Dummy
        }
public:
	void printStats();
private:
	WaitingDart* lastMarking;
};

class TimeDartReachabilitySearchPData : public TimeDartReachabilitySearch {
public:
    TimeDartReachabilitySearchPData(boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn, NonStrictMarkingBase& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<TimeDartEncodingPointer >* waiting_list)
    :TimeDartReachabilitySearch(tapn, initialMarking, query, options){
        pwList = new TimeDartPWPData(waiting_list, tapn, options.GetKBound(), tapn->NumberOfPlaces(), tapn->MaxConstant(), options.GetTrace());
    };
protected:
       virtual inline void deleteBase(NonStrictMarkingBase* base){
            delete base;
        }
};

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* NONSTRICTSEARCH_HPP_ */
