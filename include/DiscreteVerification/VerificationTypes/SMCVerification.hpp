#ifndef SMCVERIFICATION_HPP
#define SMCVERIFICATION_HPP

#include "DiscreteVerification/VerificationTypes/Verification.hpp"
#include "DiscreteVerification/DataStructures/NonStrictMarking.hpp"
#include "DiscreteVerification/Generators/SMCRunGenerator.h"
#include "Core/Query/SMCQuery.hpp"

namespace VerifyTAPN::DiscreteVerification {

class SMCVerification : public Verification<NonStrictMarking> {

    public:

        SMCVerification(TAPN::TimedArcPetriNet &tapn, NonStrictMarking &initialMarking, AST::SMCQuery *query,
                        VerificationOptions options) 
            : Verification(tapn, initialMarking, query, options)
            , runGenerator(tapn)
            , numberOfRuns(0), maxTokensSeen(0), smcSettings(query->getSmcSettings())
            { }

        virtual bool run() override;

        virtual bool executeRun();

        virtual void printStats() override;
        void printTransitionStatistics() const override;

        unsigned int maxUsedTokens() override;
        void setMaxTokensIfGreater(unsigned int i);

        virtual bool reachedRunBound();
        
        virtual void handleRunResult(const bool res) = 0;
        virtual bool mustDoAnotherRun() = 0;

        virtual void printResult() = 0;

    protected:

        SMCRunGenerator runGenerator;
        SMCSettings smcSettings;
        size_t numberOfRuns;
        unsigned int maxTokensSeen;
        unsigned long totalTime = 0;
        unsigned long totalSteps = 0; 

};

}

#endif  /* SMC_VERIFICATION_HPP */