#ifndef PROBABILITYESTIMATION_HPP
#define PROBABILITYESTIMATION_HPP

#include "DiscreteVerification/VerificationTypes/SMCVerification.hpp"
#include "Core/TAPN/WatchExpression.hpp"

namespace VerifyTAPN::DiscreteVerification {

class ProbabilityEstimation : public SMCVerification {

    public:

        ProbabilityEstimation(
            TAPN::TimedArcPetriNet &tapn, RealMarking &initialMarking, AST::SMCQuery *query, VerificationOptions options
        );

        ProbabilityEstimation(
            TAPN::TimedArcPetriNet &tapn, RealMarking &initialMarking, AST::SMCQuery *query, VerificationOptions options, unsigned int runs
        )
        : SMCVerification(tapn, initialMarking, query, options), validRuns(0), runsNeeded(runs)
        { }

        bool handleSuccessor(RealMarking* marking) override;
        void handleRunResult(const bool res, int steps, double delay, unsigned int thread_id = 0) override;
        bool mustDoAnotherRun() override;

        void prepare() override;

        float getEstimation();

        void computeChernoffHoeffdingBound(const float intervalWidth, const float confidence);

        void printStats() override;

        void printValidRunsStats();
        void printViolatingRunsStats();
        void printGlobalRunsStats();

        static void printRunsStats(const std::string category, uint64_t n, uint64_t totalSteps, double totalDelay, std::vector<uint64_t> perStep, std::vector<float> perDelay);

        void printCumulativeStats();

        void printWatchStats();

        void printResult() override;

    protected:

        uint64_t runsNeeded;
        uint64_t validRuns;
        double validRunsTime = 0;
        uint64_t validRunsSteps = 0;
        double violatingRunTime = 0;
        uint64_t violatingRunSteps = 0;

        std::vector<uint64_t> validPerStep;
        std::vector<float> validPerDelay;
        std::vector<uint64_t> violatingPerStep;
        std::vector<float> violatingPerDelay;
        float maxValidDuration = 0.0f;

};

}

#endif /*PROBABILITYESTIMATION_HPP*/
