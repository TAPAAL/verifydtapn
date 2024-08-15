#ifndef PROBABILITYESTIMATION_HPP
#define PROBABILITYESTIMATION_HPP

#include "DiscreteVerification/VerificationTypes/SMCVerification.hpp"

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
        void handleRunResult(const bool res, int steps, double delay) override;
        bool mustDoAnotherRun() override;

        void prepare() override;

        float getEstimation();

        void computeChernoffHoeffdingBound(const float intervalWidth, const float confidence);

        void printStats() override;

        void printValidRunsStats();

        void printCumulativeStats();

        void printResult() override;

    protected:

        unsigned int runsNeeded;
        unsigned int validRuns;
        double validRunsTime = 0;
        unsigned long validRunsSteps = 0;

        std::vector<int> validPerStep;
        std::vector<float> validPerDelay;
        float maxValidDuration = 0.0f;

};

}

#endif /*PROBABILITYESTIMATION_HPP*/