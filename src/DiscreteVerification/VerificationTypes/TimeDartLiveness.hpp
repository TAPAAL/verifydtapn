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
#include "../DataStructures/NonStrictMarkingBase.hpp"
#include <stack>
#include "TimeDartVerification.hpp"
#include "../DataStructures/TimeDart.hpp"
#include "../Util/IntervalOps.hpp"

namespace VerifyTAPN {
    namespace DiscreteVerification {

        class TimeDartLiveness : public TimeDartVerification {
        public:
            TimeDartLiveness(boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn, NonStrictMarkingBase& initialMarking, AST::Query* query, VerificationOptions options)
            : TimeDartVerification(tapn, options, query, initialMarking)
            {};
            TimeDartLiveness(boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn, NonStrictMarkingBase& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<WaitingDart>* waiting_list)
            : TimeDartVerification(tapn, options, query, initialMarking){
                pwList = new TimeDartLivenessPWHashMap(waiting_list);
            };
            virtual ~TimeDartLiveness();
            bool Verify();

            inline unsigned int MaxUsedTokens() {
                return pwList->maxNumTokensInAnyMarking;
            };

        protected:
            bool addToPW(NonStrictMarkingBase* marking, TimeDart* parent, int upper);
            bool canDelayForever(NonStrictMarkingBase* marking);

        protected:
            int validChildren;
            TimeDartLivenessPWBase* pwList;
        public:
            void printStats();
            void GetTrace();
        private:
            TraceList* lastMarking;
        };
        
        class TimeDartLivenessPData : public TimeDartLiveness {
        public:
            TimeDartLivenessPData(boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn, NonStrictMarkingBase& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<EncodingPointer<WaitingDart> >* waiting_list)
            : TimeDartLiveness(tapn, initialMarking, query, options){
                pwList = new TimeDartLivenessPWPData(waiting_list, tapn, options.GetKBound(), tapn->NumberOfPlaces(), tapn->MaxConstant());
            };
        };

    } /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* NONSTRICTSEARCH_HPP_ */
