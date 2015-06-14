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
#include "../SuccessorGenerator.hpp"
#include "../QueryVisitor.hpp"
#include "../DataStructures/NonStrictMarking.hpp"
#include <stack>
#include "Verification.hpp"
#include "../DataStructures/WaitingList.hpp"
#include "AbstractNaiveVerification.hpp"

namespace VerifyTAPN {
    namespace DiscreteVerification {

        class LivenessSearch : public AbstractNaiveVerification<PWListBase, NonStrictMarking> {
        public:
            LivenessSearch(TAPN::TimedArcPetriNet& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options);
            LivenessSearch(TAPN::TimedArcPetriNet& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<NonStrictMarking*>* waiting_list);
            virtual ~LivenessSearch();
            bool verify();

            virtual void deleteMarking(NonStrictMarking* m) {
                //dummy
            };

        protected:
            bool addToPW(NonStrictMarking* marking, NonStrictMarking* parent);

        public:
            void getTrace();
        protected:
            int validChildren;
        };

        class LivenessSearchPTrie : public LivenessSearch {
        public:

            LivenessSearchPTrie(TAPN::TimedArcPetriNet& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<ptriepointer<MetaData*> >* waiting_list)
            : LivenessSearch(tapn, initialMarking, query, options) {
                pwList = new PWListHybrid(tapn, waiting_list, options.getKBound(), tapn.getNumberOfPlaces(), tapn.getMaxConstant(), true, options.getTrace() == VerificationOptions::SOME_TRACE);
            };

            virtual void deleteMarking(NonStrictMarking* m) {
                delete m;
            };

        };
    } /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* NONSTRICTSEARCH_HPP_ */
