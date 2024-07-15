#include "DiscreteVerification/VerificationTypes/SMCVerification.hpp"

#include <thread>

namespace VerifyTAPN::DiscreteVerification {

bool SMCVerification::run() {
    runGenerator.prepare(&initialMarking);

    size_t n_threads = std::thread::hardware_concurrency();
    std::cout << ". Using " << n_threads << " threads..." << std::endl;

    std::vector<std::thread*> handles;
    for(int i = 0 ; i < n_threads ; i++) {
        auto handle = new std::thread([this]() {
            SMCRunGenerator generator = runGenerator.copy();
            bool continueExecution = true;
            while(continueExecution) {
                bool runRes = executeRun(&generator);
                {
                    std::lock_guard<std::mutex> lock(run_res_mutex);
                    totalTime += generator.getRunDelay();
                    totalSteps += generator.getRunSteps();
                    numberOfRuns++;
                    handleRunResult(runRes);
                    continueExecution = mustDoAnotherRun();
                }
                generator.reset();
            }
        });
        handles.push_back(handle);
    }
    for(int i = 0 ; i < n_threads ; i++) {
        handles[i]->join();
        delete handles[i];
    }

    return true;
}

bool SMCVerification::executeRun(SMCRunGenerator* generator) {
    bool runRes = false;
    if(generator == nullptr) generator = &runGenerator;
    RealMarking* newMarking = generator->getMarking();
    while(!generator->reachedEnd() && !reachedRunBound(generator)) {
        RealMarking* child = new RealMarking(*newMarking);
        runRes = handleSuccessor(child);
        if(runRes) break;
        newMarking = generator->next();
    }
    return runRes;
}

void SMCVerification::printStats() {
    std::cout << "  runs executed:\t" << numberOfRuns << std::endl;
    std::cout << "  average run length:\t" << (totalSteps / (double) numberOfRuns) << std::endl;
    std::cout << "  average run time:\t" << (totalTime / (double) numberOfRuns) << std::endl;
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

bool SMCVerification::reachedRunBound(SMCRunGenerator* generator) {
    if(generator == nullptr) generator = &runGenerator;
    switch(smcSettings.boundType) {
        case SMCSettings::TimeBound: 
            return generator->getRunDelay() >= smcSettings.bound;
        case SMCSettings::StepBound:
            return generator->getRunSteps() >= smcSettings.bound;
        default:
            return false;
    }
}

}