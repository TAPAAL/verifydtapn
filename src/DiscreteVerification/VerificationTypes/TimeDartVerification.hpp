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

            TimeDartVerification(boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn, VerificationOptions options, AST::Query* query, NonStrictMarkingBase& initialMarking) :
            query(query), options(options), tapn(tapn), initialMarking(initialMarking), exploredMarkings(0), allwaysEnabled(), successorGenerator(*tapn.get(), *this) {
                loop = false;
                deadlock = false;
                //Find the transitions which don't have input arcs
                for (TimedTransition::Vector::const_iterator iter = tapn->GetTransitions().begin(); iter != tapn->GetTransitions().end(); iter++) {
                    if ((*iter)->GetPreset().size() + (*iter)->GetTransportArcs().size() == 0) {
                        allwaysEnabled.push_back(iter->get());
                    }
                }
            }

            std::pair<int, int> calculateStart(const TAPN::TimedTransition& transition, NonStrictMarkingBase* marking);
            int calculateStop(const TAPN::TimedTransition& transition, NonStrictMarkingBase* marking);
            int maxPossibleDelay(NonStrictMarkingBase* marking);

            void printTransitionStatistics() const {
                successorGenerator.PrintTransitionStatistics(std::cout);
            }
            
            void getTrace();

            virtual inline NonStrictMarkingBase* getBase(TimeDartBase* dart){
                return dart->getBase();
            };

            virtual inline bool addToPW(NonStrictMarkingBase* m) = 0;
            
        protected:
            AST::Query* query;
            VerificationOptions options;
            boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn;
            NonStrictMarkingBase& initialMarking;
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
