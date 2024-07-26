#include "DiscreteVerification/VerificationTypes/SMCVerification.hpp"

#include <thread>

#include <chrono>

#define STEP_MS 5000

namespace VerifyTAPN::DiscreteVerification {

bool SMCVerification::parallel_run() {
    prepare();
    runGenerator.prepare(&initialMarking);
    auto start = std::chrono::steady_clock::now();

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

    auto stop = std::chrono::steady_clock::now();
    durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();

    return true;
}

bool SMCVerification::run() {
    prepare();
    runGenerator.prepare(&initialMarking);
    auto start = std::chrono::steady_clock::now();
    auto step1 = std::chrono::steady_clock::now();
    int64_t stepDuration;
    while(mustDoAnotherRun()) {
        bool runRes = executeRun();
        handleRunResult(runRes);
        
        totalTime += runGenerator.getRunDelay();
        totalSteps += runGenerator.getRunSteps();
        numberOfRuns++;
        runGenerator.reset();
        
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