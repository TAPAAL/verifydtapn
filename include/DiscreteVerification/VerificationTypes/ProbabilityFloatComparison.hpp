#ifndef PROBABILITYFLOATCOMPARISON_HPP
#define PROBABILITYFLOATCOMPARISON_HPP

#include "DiscreteVerification/VerificationTypes/SMCVerification.hpp"

namespace VerifyTAPN::DiscreteVerification {

class ProbabilityFloatComparison : public SMCVerification {

    public:

        ProbabilityFloatComparison(
            TAPN::TimedArcPetriNet &tapn, NonStrictMarking &initialMarking, AST::SMCQuery *query, VerificationOptions options
        );

        bool handleSuccessor(NonStrictMarking* marking) override;
        void handleRunResult(const bool res) override;
        bool mustDoAnotherRun() override;

        bool getResult();

        void computeAcceptingBounds(const float alpha, const float beta);
        void computeIndifferenceRegion(const float p, const float sigma0, const float sigma1);

        void printStats() override;

        void printResult() override;

    protected:

        float ratio;
        float p0;
        float p1;
        float boundH0;
        float boundH1;
        bool result;
        unsigned int validRuns;

};

}

#endif /*PROBABILITYFLOATCOMPARISON_HPP*/