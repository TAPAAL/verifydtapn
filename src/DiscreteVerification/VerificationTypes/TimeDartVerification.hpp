#ifndef TIMEDARTVERIFICATION_HPP_
#define TIMEDARTVERIFICATION_HPP_

#include "../../Core/TAPN/TAPN.hpp"
#include "../DataStructures/NonStrictMarkingBase.hpp"
#include "../Util/IntervalOps.hpp"
#include "../TimeDartSuccessorGenerator.hpp"
#include "Verification.hpp"
#include "../DataStructures/TimeDart.hpp"
#include <stack>

namespace VerifyTAPN {
    namespace DiscreteVerification {

        using namespace rapidxml;

        typedef pair<NonStrictMarkingBase*, int> TraceList;

        class TimeDartVerification : public Verification<NonStrictMarkingBase> {
        public:

            TimeDartVerification(TAPN::TimedArcPetriNet& tapn, VerificationOptions options, AST::Query* query, NonStrictMarkingBase& initialMarking);

            std::pair<int, int> calculateStart(const TAPN::TimedTransition& transition, NonStrictMarkingBase* marking);
            int calculateStop(const TAPN::TimedTransition& transition, NonStrictMarkingBase* marking);
            int maxPossibleDelay(NonStrictMarkingBase* marking);

            void printTransitionStatistics() const {
                successorGenerator.printTransitionStatistics(std::cout);
            }
            
            void getTrace();

            virtual inline NonStrictMarkingBase* getBase(TimeDartBase* dart){
                return dart->getBase();
            };

            virtual inline bool handleSuccessor(NonStrictMarkingBase* m) = 0;
            
        protected:
            int exploredMarkings;
            vector<const TAPN::TimedTransition*> allwaysEnabled;
            bool loop;         
            bool deadlock;
            WaitingDart* lastMarking;
            TimeDartSuccessorGenerator successorGenerator;

        };
    }
}

#endif
