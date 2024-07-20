#include "DiscreteVerification/VerificationTypes/SMCVerification.hpp"

#include <chrono>

#define STEP_MS 5000

namespace VerifyTAPN::DiscreteVerification {

bool SMCVerification::run() {
    prepare();
    runGenerator.prepare(&initialMarking);
    auto start = std::chrono::steady_clock::now();
    auto step1 = std::chrono::steady_clock::now();
    int64_t stepDuration;
    while(mustDoAnotherRun()) {
        bool runRes = executeRun();
        handleRunResult(runRes);
        numberOfRuns++;
        if(numberOfRuns % 100 != 0) continue;
        auto step2 = std::chrono::steady_clock::now();
        stepDuration = std::chrono::duration_cast<std::chrono::milliseconds>(step2 - step1).count();
        if(stepDuration >= STEP_MS) {
            step1 = step2;
            stepDuration = std::chrono::duration_cast<std::chrono::milliseconds>(step2 - start).count();
            std::cout << ". Duration : " << stepDuration << "ms ; Runs executed : " << numberOfRuns << std::endl;
        }
    }
    auto stop = std::chrono::steady_clock::now();
    durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
    return true;
}

bool SMCVerification::executeRun() {
    bool runRes = false;
    RealMarking* newMarking = runGenerator.getMarking();
    while(!runGenerator.reachedEnd() && !reachedRunBound()) {
        RealMarking* child = new RealMarking(*newMarking);
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
    std::cout << "  average run length:\t" << (totalSteps / (double) numberOfRuns) << std::endl;
    std::cout << "  average run time:\t" << (totalTime / (double) numberOfRuns) << std::endl;
    std::cout << "  verification time:\t" << ((double) durationMs / 1000.0) << "s" << std::endl;
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