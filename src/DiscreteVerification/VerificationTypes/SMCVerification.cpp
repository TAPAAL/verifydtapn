#include "DiscreteVerification/VerificationTypes/SMCVerification.hpp"

namespace VerifyTAPN::DiscreteVerification {

bool SMCVerification::run() {
    runGenerator.prepare(&initialMarking);
    while(mustDoAnotherRun()) {
        bool runRes = executeRun();
        handleRunResult(runRes);
        numberOfRuns++;
    }
    return true;
}

bool SMCVerification::executeRun() {
    bool runRes = false;
    NonStrictMarkingBase* newMarking = new NonStrictMarkingBase(initialMarking);
    while(!runGenerator.reachedEnd() && !reachedRunBound()) {
        NonStrictMarking* child = new NonStrictMarking(*newMarking);
        setMaxTokensIfGreater(child->size());
        runRes = handleSuccessor(child);
        if(runRes) break;
        newMarking = runGenerator.next();
    }
    totalTime += runGenerator.getRunDelay();
    totalSteps += runGenerator.getRunSteps();
    runGenerator.reset();
    return runRes;
}

void SMCVerification::printStats() {
    std::cout << "  runs executed:\t" << numberOfRuns << std::endl;
    std::cout << "  average run length:\t" << (totalSteps / (float) numberOfRuns) << std::endl;
    std::cout << "  average run time:\t" << (totalTime / (float) numberOfRuns) << std::endl;
}

void SMCVerification::printTransitionStatistics() const {
    runGenerator.printTransitionStatistics(std::cout);
}

unsigned int SMCVerification::maxUsedTokens() {
    return maxTokensSeen;
}

void SMCVerification::setMaxTokensIfGreater(unsigned int i) {
    if(i > maxTokensSeen) {
        maxTokensSeen = i;
    }
}

bool SMCVerification::reachedRunBound() {
    switch(smcSettings.boundType) {
        case SMCSettings::TimeBound: 
            return runGenerator.getRunDelay() >= smcSettings.bound;
        case SMCSettings::StepBound:
            return runGenerator.getRunSteps() >= smcSettings.bound;
        default:
            return false;
    }
}

}