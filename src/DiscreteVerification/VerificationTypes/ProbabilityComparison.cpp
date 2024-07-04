#include "DiscreteVerification/VerificationTypes/ProbabilityComparison.hpp"
#include "DiscreteVerification/QueryVisitor.hpp"

#include <iostream>

namespace VerifyTAPN::DiscreteVerification {

ProbabilityComparison::ProbabilityComparison(
    TAPN::TimedArcPetriNet &tapn, NonStrictMarking &initialMarking, AST::SMCQuery *query_1, AST::SMCQuery *query_2, VerificationOptions options
) : Verification(tapn, initialMarking, query_1, options), maxTokensSeen(0), numberOfRuns(0), result(0), mayBeIndifferent(true),
    acceptingRuns(0), ratio_indifferent(0), finished(false),
    runGenerator(tapn)
{
    this->query_1 = query_1;
    this->query_2 = query_2;
}

bool ProbabilityComparison::run() {
    runGenerator.prepare(&initialMarking);
    while(mustDoAnotherRun()) {
        bool runResQ1 = executeRunFor(query_1);
        bool runResQ2 = executeRunFor(query_2);
        handleRunsResults(runResQ1, runResQ2);
        numberOfRuns += 2;
    }
    return true;
}

bool ProbabilityComparison::executeRunFor(AST::SMCQuery* query) {
    bool runRes = false;
    RealMarking* newMarking = new RealMarking(initialMarking);
    while(!runGenerator.reachedEnd() && !reachedRunBound(query)) {
        NonStrictMarking* child = new NonStrictMarking(newMarking->generateImage());
        setMaxTokensIfGreater(child->size());
        runRes = handleSuccessor(query, child);
        if(runRes) break;
        newMarking = runGenerator.next();
    }
    totalTime += runGenerator.getRunDelay();
    totalSteps += runGenerator.getRunSteps();
    runGenerator.reset();
    return runRes;
}

void ProbabilityComparison::handleRunsResults(bool resQ1, bool resQ2) {
    if(query_1->getQuantifier() == PG) resQ1 = !resQ1;
    if(query_2->getQuantifier() == PG) resQ2 = !resQ2;
    bool eq = resQ1 == resQ2;
    if(mayBeIndifferent) {
        ratio_indifferent += eq ? log(p1 / p0) : log((1 - p1) / (1 - p0));
        if(ratio_indifferent <= boundIndiffH0) {
            result = INDIFFERENT;
            finished = true;
            return;
        }
        if(ratio_indifferent >= boundIndiffH1) {
            mayBeIndifferent = false;
        }
    }
    if(!eq) {
        boundH0 += boundIncr;
        boundH1 += boundIncr;
        if(resQ2) { // And therefore !resQ1
            acceptingRuns++;
        }
        if(acceptingRuns <= boundH0) {
            result = ACCEPT;
            finished = true;
        } else if(acceptingRuns >= boundH1) {
            result = REJECT;
            finished = true;
        }
    }
}

bool ProbabilityComparison::handleSuccessor(AST::SMCQuery* current_query, NonStrictMarking* marking) {
    marking->cut(placeStats);

    QueryVisitor<NonStrictMarking> checker(*marking, tapn);
    AST::BoolResult context;
    current_query->accept(checker, context);

    delete marking;

    return context.value;
}

bool ProbabilityComparison::mustDoAnotherRun() {
    return !finished;
}

int ProbabilityComparison::getResult() {
    return result;
}

void ProbabilityComparison::computeAcceptingBounds(const float alpha, const float beta) {
    boundIndiffH0 = log(beta / (1 - alpha));
    boundIndiffH1 = log((1 - beta) / alpha);

}

void ProbabilityComparison::computeIndifferenceRegion(const float p, const float sigma0, const float sigma1) {

}

bool ProbabilityComparison::reachedRunBound(AST::SMCQuery* current_query) {
    switch(current_query->getSmcSettings().boundType) {
        case SMCSettings::TimeBound: 
            return runGenerator.getRunDelay() >= current_query->getSmcSettings().bound;
        case SMCSettings::StepBound:
            return runGenerator.getRunSteps() >= current_query->getSmcSettings().bound;
        default:
            return false;
    }
}

void ProbabilityComparison::printStats() {

}

void ProbabilityComparison::printResult() {
    int result = getResult();
    std::string resultStr =    result == REJECT ? "Rejected" : 
                            result == ACCEPT ? "Accepted" :
                            "Undecided";
    std::cout << "Probability comparison:" << std::endl;
    std::cout << "\tHypothesis is " << resultStr << std::endl;
}

unsigned int ProbabilityComparison::maxUsedTokens() {
    return maxTokensSeen;
}

void ProbabilityComparison::setMaxTokensIfGreater(unsigned int i) {
    if(i > maxTokensSeen) {
        maxTokensSeen = i;
    }
}



}
