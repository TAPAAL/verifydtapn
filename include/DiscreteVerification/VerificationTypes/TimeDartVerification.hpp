#ifndef TIMEDARTVERIFICATION_HPP_
#define TIMEDARTVERIFICATION_HPP_

#include "Core/TAPN/TAPN.hpp"
#include "DiscreteVerification/DataStructures/NonStrictMarkingBase.hpp"
#include "DiscreteVerification/Util/IntervalOps.hpp"
#include "Verification.hpp"
#include "DiscreteVerification/DataStructures/TimeDart.hpp"
#include "DiscreteVerification/Generators/Generator.h"
#include "DiscreteVerification/Generators/ReducingGenerator.hpp"
#include <stack>

namespace VerifyTAPN { namespace DiscreteVerification {

    using namespace rapidxml;

    typedef std::pair<NonStrictMarkingBase *, int> TraceList;

    class TimeDartVerification : public Verification<NonStrictMarkingBase> {
    public:

        TimeDartVerification(TAPN::TimedArcPetriNet &tapn, const VerificationOptions &options, AST::Query *query,
                             NonStrictMarkingBase &initialMarking);

        std::pair<int, int> calculateStart(const TAPN::TimedTransition &transition, NonStrictMarkingBase *marking);

        int calculateStop(const TAPN::TimedTransition &transition, NonStrictMarkingBase *marking);

        int maxPossibleDelay(NonStrictMarkingBase *marking);

        void printTransitionStatistics() const {
            successorGenerator.printTransitionStatistics(std::cout);
        }

        void getTrace();

        virtual inline NonStrictMarkingBase *getBase(TimeDartBase *dart) {
            return dart->getBase();
        };

        virtual inline bool handleSuccessor(NonStrictMarkingBase *m) {
            assert(false);
            return false;
        }

    protected:
        int exploredMarkings;
        std::vector<const TAPN::TimedTransition *> allwaysEnabled{};
        bool loop;
        bool deadlock;
        WaitingDart *lastMarking{};
        Generator successorGenerator;

        bool generateAndInsertSuccessors(NonStrictMarkingBase &marking, const TAPN::TimedTransition &transition);
    };
} }

#endif
