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
#include "../../Core/TAPN/TAPN.hpp"
#include "../../Core/QueryParser/AST.hpp"
#include "../../Core/VerificationOptions.hpp"
#include "../QueryVisitor.hpp"
#include "../DataStructures/NonStrictMarkingBase.hpp"
#include <stack>
#include <utility>
#include "TimeDartVerification.hpp"
#include "../DataStructures/TimeDart.hpp"
#include "../Util/IntervalOps.hpp"

namespace VerifyTAPN::DiscreteVerification {

        class TimeDartLiveness : public TimeDartVerification {
        public:

            TimeDartLiveness(TAPN::TimedArcPetriNet &tapn, NonStrictMarkingBase &initialMarking, AST::Query *query,
                             VerificationOptions options)
                    : TimeDartVerification(tapn, std::move(options), query, initialMarking) {
            };

            TimeDartLiveness(TAPN::TimedArcPetriNet &tapn, NonStrictMarkingBase &initialMarking, AST::Query *query,
                             const VerificationOptions& options, WaitingList<WaitingDart *> *waiting_list)
                    : TimeDartVerification(tapn, options, query, initialMarking) {
                pwList = new TimeDartLivenessPWHashMap(options, waiting_list);
            };

            virtual ~TimeDartLiveness();

            bool run();

            inline unsigned int maxUsedTokens() {
                return pwList->maxNumTokensInAnyMarking;
            };

            inline bool handleSuccessor(NonStrictMarkingBase *m) override {
                return addToPW(m, tmpdart, tmpupper);
            };
        protected:
            WaitingDart *tmpdart{};
            int tmpupper{};

            bool addToPW(NonStrictMarkingBase *marking, WaitingDart *parent, int upper);

            bool canDelayForever(NonStrictMarkingBase *marking);

        protected:
            int validChildren{};
            TimeDartLivenessPWBase *pwList{};

            virtual inline void deleteBase(NonStrictMarkingBase *base) {
                //
            };


        public:
            void printStats();
        };

        class TimeDartLivenessPData : public TimeDartLiveness {
        public:

            TimeDartLivenessPData(TAPN::TimedArcPetriNet &tapn, NonStrictMarkingBase &initialMarking, AST::Query *query,
                                  const VerificationOptions& options,
                                  WaitingList<std::pair<WaitingDart *, ptriepointer_t<LivenessDart *> > > *waiting_list)
                    : TimeDartLiveness(tapn, initialMarking, query, options) {
                pwList = new TimeDartLivenessPWPData(options, waiting_list, tapn, tapn.getNumberOfPlaces(),
                                                     tapn.getMaxConstant());
            };

        protected:
            WaitingDart *tmpdart{};
            int tmpupper{};

            inline void deleteBase(NonStrictMarkingBase *base) override {
                delete base;
            };

            inline NonStrictMarkingBase *getBase(TimeDartBase *dart) override {
                auto *eld = (EncodedLivenessDart *) dart;
                return ((TimeDartLivenessPWPData *) pwList)->decode(eld->encoding);
            };
        };

    } /* namespace VerifyTAPN */
#endif /* NONSTRICTSEARCH_HPP_ */
