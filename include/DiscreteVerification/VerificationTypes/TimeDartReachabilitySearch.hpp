/*
 * NonStrictSearch.hpp
 *
 *  Created on: 26/04/2012
 *      Author: MathiasGS
 */

#ifndef TIMEDARTREACHABILITYSEARCH_HPP_
#define TIMEDARTREACHABILITYSEARCH_HPP_

#include "DiscreteVerification/DataStructures/TimeDart.hpp"
#include "DiscreteVerification/DataStructures/TimeDartPWList.hpp"
#include "Core/TAPN/TAPN.hpp"
#include "Core/QueryParser/AST.hpp"
#include "Core/VerificationOptions.hpp"
#include "DiscreteVerification/QueryVisitor.hpp"
#include "DiscreteVerification/DataStructures/NonStrictMarkingBase.hpp"
#include <stack>
#include <utility>
#include "TimeDartVerification.hpp"
#include "DiscreteVerification/DataStructures/TimeDart.hpp"
#include "../Util/IntervalOps.hpp"

namespace VerifyTAPN::DiscreteVerification {

    using namespace rapidxml;

    class TimeDartReachabilitySearch : public TimeDartVerification {
    public:
        TimeDartReachabilitySearch(TAPN::TimedArcPetriNet &tapn, NonStrictMarkingBase &initialMarking,
                                   AST::Query *query, VerificationOptions options)
                : TimeDartVerification(tapn, std::move(options), query, initialMarking), trace(10000) {};

        TimeDartReachabilitySearch(TAPN::TimedArcPetriNet &tapn, NonStrictMarkingBase &initialMarking,
                                   AST::Query *query, const VerificationOptions &options,
                                   WaitingList<TimeDartBase *> *waiting_list)
                : TimeDartVerification(tapn, options, query, initialMarking), trace(10000) {
            pwList = new TimeDartPWHashMap(waiting_list, options.getTrace());
        };

        virtual ~TimeDartReachabilitySearch();

        bool run();

        inline unsigned int maxUsedTokens() { return pwList->maxNumTokensInAnyMarking; };

        inline bool handleSuccessor(NonStrictMarkingBase *m) override {
            return handleSuccessor(m, tmpdart, tmpupper);
        };
    protected:
        WaitingDart *tmpdart{};
        int tmpupper{};

        bool handleSuccessor(NonStrictMarkingBase *marking, WaitingDart *parent, int upper);

    protected:
        int validChildren{};
        google::sparse_hash_map<NonStrictMarkingBase *, TraceList> trace{};
        TimeDartPWBase *pwList{};
        vector<const TAPN::TimedTransition *> allwaysEnabled{};

        virtual inline void deleteBase(NonStrictMarkingBase *base) {
            // Dummy
        }

    public:
        void printStats();
    };

    class TimeDartReachabilitySearchPData : public TimeDartReachabilitySearch {
    public:
        TimeDartReachabilitySearchPData(TAPN::TimedArcPetriNet &tapn, NonStrictMarkingBase &initialMarking,
                                        AST::Query *query, const VerificationOptions &options,
                                        WaitingList<TimeDartEncodingPointer> *waiting_list)
                : TimeDartReachabilitySearch(tapn, initialMarking, query, options) {
            pwList = new TimeDartPWPData(waiting_list, tapn, options.getKBound(), tapn.getNumberOfPlaces(),
                                         tapn.getMaxConstant(), options.getTrace());
        };
    protected:
        inline void deleteBase(NonStrictMarkingBase *base) override {
            delete base;
        };

        inline NonStrictMarkingBase *getBase(TimeDartBase *dart) override {
            auto *erd = (EncodedReachabilityTraceableDart *) dart;

            return ((TimeDartPWPData *) pwList)->decode(erd->encoding);
        };
    };

} /* namespace VerifyTAPN */
#endif /* NONSTRICTSEARCH_HPP_ */
