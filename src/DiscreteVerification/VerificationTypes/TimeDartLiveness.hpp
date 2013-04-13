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
            : TimeDartVerification(tapn, options, query, initialMarking) {
            };

            TimeDartLiveness(boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn, NonStrictMarkingBase& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<WaitingDart>* waiting_list)
            : TimeDartVerification(tapn, options, query, initialMarking) {
                pwList = new TimeDartLivenessPWHashMap( options, waiting_list);
            };
            virtual ~TimeDartLiveness();
            bool Verify();

            inline unsigned int MaxUsedTokens() {
                return pwList->maxNumTokensInAnyMarking;
            };
            virtual inline bool addToPW(NonStrictMarkingBase* m){
                return addToPW(m,tmpdart, tmpupper);
            };
        protected:
            WaitingDart* tmpdart;
            int tmpupper;
            bool addToPW(NonStrictMarkingBase* marking, WaitingDart* parent, int upper);
            bool canDelayForever(NonStrictMarkingBase* marking);

        protected:
            int validChildren;
            TimeDartLivenessPWBase* pwList;

            virtual inline void deleteBase(NonStrictMarkingBase* base) {
                //
            };
           
            
        public:
            void printStats();
        };

        class TimeDartLivenessPData : public TimeDartLiveness {
        public:

            TimeDartLivenessPData(boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn, NonStrictMarkingBase& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<EncodingPointer<WaitingDart> >* waiting_list)
            : TimeDartLiveness(tapn, initialMarking, query, options) {
                pwList = new TimeDartLivenessPWPData(options, waiting_list, tapn, tapn->NumberOfPlaces(), tapn->MaxConstant());
            };

        protected:
            WaitingDart* tmpdart;
            int tmpupper;
            virtual inline void deleteBase(NonStrictMarkingBase* base) {
                delete base;
            };
            
            virtual inline NonStrictMarkingBase* getBase(TimeDartBase* dart){
                EncodedLivenessDart* eld = (EncodedLivenessDart*)dart;
                EncodingPointer<LivenessDart>* ep = (EncodingPointer<LivenessDart>*)(eld->encoding);
                return ((TimeDartLivenessPWPData*)pwList)->Decode(ep);
            };
        };

    } /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* NONSTRICTSEARCH_HPP_ */
