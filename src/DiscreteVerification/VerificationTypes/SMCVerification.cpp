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
                    handleRunResult(runRes, generator.getRunSteps(), generator.getRunDelay());
                    if(mustSaveTrace()) handleTrace(runRes, &generator);
                    generator.recordTrace = mustSaveTrace();
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
    durationNs = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count();

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
        handleRunResult(runRes, runGenerator.getRunSteps(), runGenerator.getRunDelay());
        if(mustSaveTrace()) handleTrace(runRes);
        runGenerator.recordTrace = mustSaveTrace();
        
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
    durationNs = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count();
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
    std::cout << "  verification time:\t" << ((double) durationNs / 1.0E9) << "s" << std::endl;
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
    return 
        generator->getRunDelay() >= smcSettings.timeBound ||
        generator->getRunSteps() >= smcSettings.stepBound;
}

void SMCVerification::handleTrace(const bool runRes, SMCRunGenerator* generator) {
    if(generator == nullptr) generator = &runGenerator; 
    bool valid = query->getQuantifier() == PG ? !runRes : runRes;
    VerificationOptions::SMCTracesToSave type = options.getTracesToSave();
    if(type == VerificationOptions::ANY_TRACE) {
        saveTrace(generator);
    } else if(type == VerificationOptions::SATISFYING_TRACES && valid) {
        saveTrace(generator);
    } else if(type == VerificationOptions::UNSATISFYING_TRACES && !valid) {
        saveTrace(generator);
    }
}

void SMCVerification::saveTrace(SMCRunGenerator* generator) {
    if(generator == nullptr) generator = &runGenerator;
    traces.push_back(generator->getTrace());
}

void SMCVerification::getTrace() {

}

void SMCVerification::printHumanTrace(RealMarking *m, std::stack<RealMarking *> &stack, AST::Quantifier query) {

}

void SMCVerification::printXMLTrace(RealMarking *m, std::stack<RealMarking *> &stack, AST::Query *query, TAPN::TimedArcPetriNet &tapn) {

}

rapidxml::xml_node<> *SMCVerification::createTransitionNode(RealMarking *old, RealMarking *current, rapidxml::xml_document<> &doc) {

}

void SMCVerification::createTransitionSubNodes(RealMarking *old, RealMarking *current, rapidxml::xml_document<> &doc,
                                rapidxml::xml_node<> *transitionNode, const TAPN::TimedPlace &place,
                                const TAPN::TimeInterval &interval, int weight) {}

rapidxml::xml_node<> *
SMCVerification::createTokenNode(rapidxml::xml_document<> &doc, const TAPN::TimedPlace &place, const RealToken &token) {
    using namespace rapidxml;
    xml_node<> *tokenNode = doc.allocate_node(node_element, "token");
    xml_attribute<> *placeAttribute = doc.allocate_attribute("place",
                                                                doc.allocate_string(place.getName().c_str()));
    tokenNode->append_attribute(placeAttribute);
    auto str = std::to_string(token.getAge() * tapn.getGCD());
    xml_attribute<> *ageAttribute = doc.allocate_attribute("age", doc.allocate_string(
            str.c_str()));
    tokenNode->append_attribute(ageAttribute);
    if (place.getMaxConstant() < token.getAge()) {
        xml_attribute<> *gtAttribute = doc.allocate_attribute("greaterThanOrEqual",
                                                                doc.allocate_string("true"));
        tokenNode->append_attribute(gtAttribute);
    } else {
        xml_attribute<> *gtAttribute = doc.allocate_attribute("greaterThanOrEqual",
                                                                doc.allocate_string("false"));
        tokenNode->append_attribute(gtAttribute);
    }

    return tokenNode;
}

}