#ifndef PROBABILITYESTIMATION_HPP
#define PROBABILITYESTIMATION_HPP

#include "DiscreteVerification/VerificationTypes/SMCVerification.hpp"

namespace VerifyTAPN::DiscreteVerification {

class ProbabilityEstimation : public SMCVerification {

    public:

        ProbabilityEstimation(
            TAPN::TimedArcPetriNet &tapn, RealMarking &initialMarking, AST::SMCQuery *query, VerificationOptions options
        );

        bool handleSuccessor(RealMarking* marking) override;
        void handleRunResult(const bool res) override;
        bool mustDoAnotherRun() override;

        float getEstimation();

        void computeChernoffHoeffdingBound(const float intervalWidth, const float confidence);

        void printStats() override;

        void printResult() override;

    protected:

        unsigned int runsNeeded;
        unsigned int validRuns;

};

}

#endif /*PROBABILITYESTIMATION_HPP*/