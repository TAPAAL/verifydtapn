/*
 * NonStrictSearch.hpp
 *
 *  Created on: 26/04/2012
 *      Author: MathiasGS
 */

#ifndef LIVENESSSEARCH_HPP_
#define LIVENESSSEARCH_HPP_

#include "../DataStructures/PWList.hpp"
#include "../../Core/TAPN/TAPN.hpp"
#include "../../Core/QueryParser/AST.hpp"
#include "../../Core/VerificationOptions.hpp"
#include "../../Core/TAPN/TimedPlace.hpp"
#include "../../Core/TAPN/TimedTransition.hpp"
#include "../../Core/TAPN/TimedInputArc.hpp"
#include "../../Core/TAPN/TransportArc.hpp"
#include "../../Core/TAPN/InhibitorArc.hpp"
#include "../../Core/TAPN/OutputArc.hpp"
#include "../QueryVisitor.hpp"
#include "../DataStructures/NonStrictMarking.hpp"
#include <stack>
#include <utility>
#include "Verification.hpp"
#include "../DataStructures/WaitingList.hpp"
#include "AbstractNaiveVerification.hpp"

namespace VerifyTAPN::DiscreteVerification {

    class LivenessSearch : public AbstractNaiveVerification<PWListBase, NonStrictMarking, Generator> {
    public:
        LivenessSearch(TAPN::TimedArcPetriNet &tapn, NonStrictMarking &initialMarking, AST::Query *query,
                       const VerificationOptions &options);

        LivenessSearch(TAPN::TimedArcPetriNet &tapn, NonStrictMarking &initialMarking, AST::Query *query,
                       const VerificationOptions &options, WaitingList<NonStrictMarking *> *waiting_list);

        virtual ~LivenessSearch();

        bool run();

        virtual void deleteMarking(NonStrictMarking *m) {
            //dummy
        };

    protected:
        bool handleSuccessor(NonStrictMarking *marking, NonStrictMarking *parent);

    public:
        void getTrace();

    protected:
        int validChildren{};
    };

    class LivenessSearchPTrie : public LivenessSearch {
    public:

        LivenessSearchPTrie(TAPN::TimedArcPetriNet &tapn, NonStrictMarking &initialMarking, AST::Query *query,
                            VerificationOptions options, WaitingList<ptriepointer_t<MetaData *> > *waiting_list)
                : LivenessSearch(tapn, initialMarking, query, std::move(options)) {
            pwList = new PWListHybrid(tapn, waiting_list, options.getKBound(), tapn.getNumberOfPlaces(),
                                      tapn.getMaxConstant(), true,
                                      options.getTrace() == VerificationOptions::SOME_TRACE);
        };

        void deleteMarking(NonStrictMarking *m) override {
            delete m;
        };

    };
} /* namespace VerifyTAPN */
#endif /* NONSTRICTSEARCH_HPP_ */
