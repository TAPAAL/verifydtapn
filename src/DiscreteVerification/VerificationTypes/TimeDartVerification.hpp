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

            TimeDartVerification(TAPN::TimedArcPetriNet& tapn, VerificationOptions options, AST::Query* query, NonStrictMarkingBase& initialMarking) :
            Verification<NonStrictMarkingBase>(tapn, initialMarking, query, options), exploredMarkings(0), allwaysEnabled(), successorGenerator(tapn, *this) {
                loop = false;
                deadlock = false;
                //Find the transitions which don't have input arcs
                for (TimedTransition::Vector::const_iterator iter = tapn.getTransitions().begin(); iter != tapn.getTransitions().end(); iter++) {
                    if ((*iter)->getPreset().size() + (*iter)->getTransportArcs().size() == 0) {
                        allwaysEnabled.push_back((*iter));
                    }
                    if((*iter)->isUrgent()){    // no implementation for urgency in timedart engine yet
                        cout << "The TimeDart engine cannot handle urgent transitions" << endl;
                        exit(1);
                    }
                }
            }

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

            virtual inline bool addToPW(NonStrictMarkingBase* m) = 0;
            
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
