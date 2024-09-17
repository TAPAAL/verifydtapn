#ifndef SMC_TRACES_GENERATOR
#define SMC_TRACES_GENERATOR

#include "DiscreteVerification/VerificationTypes/SMCVerification.hpp"

namespace VerifyTAPN::DiscreteVerification {

class SMCTracesGenerator : public SMCVerification {

    public:

        SMCTracesGenerator(
            TAPN::TimedArcPetriNet &tapn, RealMarking &initialMarking, AST::SMCQuery *query, VerificationOptions options
        );

        SMCTracesGenerator(
            TAPN::TimedArcPetriNet &tapn, RealMarking &initialMarking, AST::SMCQuery *query, VerificationOptions options, unsigned int runs
        )
        : SMCVerification(tapn, initialMarking, query, options)
        { }

        bool handleSuccessor(RealMarking* marking) override;
        void handleRunResult(const bool res, int steps, double delay) override;
        bool mustDoAnotherRun() override;

        void printResult() override;

};

}

#endif