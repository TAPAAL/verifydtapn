#ifndef PROBABILITYCOMPARISON_HPP
#define PROBABILITYCOMPARISON_HPP

#include "DiscreteVerification/VerificationTypes/Verification.hpp"
#include "DiscreteVerification/Generators/SMCRunGenerator.h"

#define INDIFFERENT 0
#define ACCEPT 1
#define REJECT -1

namespace VerifyTAPN::DiscreteVerification {

class ProbabilityComparison : public Verification<NonStrictMarkingBase> {

    public:

        ProbabilityComparison(
            TAPN::TimedArcPetriNet &tapn, NonStrictMarking &initialMarking, AST::Query *query_1, AST::Query *query_2, VerificationOptions options
        );

        virtual bool run();
        virtual bool executeRunFor(AST::Query* query);

        bool handleSuccessor(AST::Query* query, NonStrictMarking* marking);

        void handleRunsResults(bool resQ1, bool resQ2);

        bool reachedRunBound(AST::Query* query);

        bool mustDoAnotherRun();

        int getResult();

        void computeAcceptingBounds(const float alpha, const float beta);
        void computeIndifferenceRegion(const float p, const float sigma0, const float sigma1);

        void printStats();

        void printResult();

        unsigned int maxUsedTokens() override;
        void setMaxTokensIfGreater(unsigned int i);

    protected:

        SMCRunGenerator runGenerator;
        AST::Query* query_1;
        AST::Query* query_2;
        size_t numberOfRuns;

        bool mayBeIndifferent;
        float ratio_indifferent;
        float p0;
        float p1;
        float boundIndiffH0;
        float boundIndiffH1;

        float u0;
        float u1;
        float boundH0;
        float boundH1;
        float boundIncr;
        int acceptingRuns;

        int result;

        bool finished;

        unsigned int validRunsQ1;
        unsigned int validRunsQ2;
        unsigned int maxTokensSeen;
        unsigned long totalTime = 0;
        unsigned long totalSteps = 0;

};

}

#endif /* PROBABILITYCOMPARISON_HPP */