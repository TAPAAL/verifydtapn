#ifndef PROBABILITYFLOATCOMPARISON_HPP
#define PROBABILITYFLOATCOMPARISON_HPP

#include "DiscreteVerification/VerificationTypes/SMCVerification.hpp"

namespace VerifyTAPN::DiscreteVerification {

class ProbabilityFloatComparison : public SMCVerification {

    public:

        ProbabilityFloatComparison(
            TAPN::TimedArcPetriNet &tapn, NonStrictMarking &initialMarking, AST::Query *query, VerificationOptions options
        );

        bool handleSuccessor(NonStrictMarking* marking) override;
        void handleRunResult(const bool res) override;
        bool mustDoAnotherRun() override;

        bool getResult();

        void computeAcceptingBounds(const float alpha, const float beta);

        void printStats() override;

        void printResult();

    protected:

        float bound1;
        float bound2;
        unsigned int validRuns;

};

}

#endif /*PROBABILITYFLOATCOMPARISON_HPP*/