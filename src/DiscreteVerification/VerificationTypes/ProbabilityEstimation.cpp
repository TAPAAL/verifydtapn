#include "DiscreteVerification/VerificationTypes/ProbabilityEstimation.hpp"
#include "DiscreteVerification/QueryVisitor.hpp"

#include <math.h>

namespace VerifyTAPN::DiscreteVerification {

ProbabilityEstimation::ProbabilityEstimation(
    TAPN::TimedArcPetriNet &tapn, RealMarking &initialMarking, AST::SMCQuery *query, VerificationOptions options
)
: SMCVerification(tapn, initialMarking, query, options), validRuns(0)
{
    computeChernoffHoeffdingBound(smcSettings.estimationIntervalWidth, smcSettings.confidence);
}

bool ProbabilityEstimation::mustDoAnotherRun() {
    return numberOfRuns < runsNeeded;
}

void ProbabilityEstimation::prepare()
{
    std::cout << "Need to execute " << runsNeeded << " runs to produce estimation" << std::endl;
}

void ProbabilityEstimation::handleRunResult(const bool res, int steps, double delay)
{
    bool valid = (query->getQuantifier() == PF && res) || (query->getQuantifier() == PG && !res);
    validRuns += valid ? 1 : 0;
    if(!valid || !options.mustPrintCumulative()) return;
    int delayCap = (int) ceil(delay);
    if(validPerStep.size() <= steps) {
        validPerStep.resize(steps + 1, 0);
    }
    if(validPerDelay.size() <= delayCap) {
        validPerDelay.resize(delayCap + 1, 0);
    }
    validPerStep[steps] += 1;
    validPerDelay[delayCap] += 1;
}

bool ProbabilityEstimation::handleSuccessor(RealMarking* marking) {
    QueryVisitor<RealMarking> checker(*marking, tapn);
    AST::BoolResult context;
    query->accept(checker, context);

    delete marking;

    return context.value;
}

float ProbabilityEstimation::getEstimation() {
    return ((float) validRuns) / numberOfRuns;
}

void ProbabilityEstimation::computeChernoffHoeffdingBound(const float intervalWidth, const float confidence) {
    // https://link.springer.com/content/pdf/10.1007/b94790.pdf p.78-79
    float bound = log(2.0 / (1 - confidence)) / (2.0 * pow(intervalWidth, 2));
    runsNeeded = (unsigned int) ceil(bound);
}

void ProbabilityEstimation::printStats() {
    SMCVerification::printStats();
    std::cout << "  valid runs:\t" << validRuns << std::endl;
    if(!options.mustPrintCumulative()) return;
    std::cout << "  cumulative probability / step :" << std::endl;
    double acc = 0;
    for(int i = 0 ; i < validPerStep.size() ; i++) {
        acc += validPerStep[i] / (double) numberOfRuns;
        std::cout << i << ":" << acc << ";";
    }
    std::cout << std::endl << "  cumulative probability / delay :" << std::endl;
    acc = 0;
    for(int i = 0 ; i < validPerDelay.size() ; i++) {
        acc += validPerDelay[i] / (double) numberOfRuns;
        std::cout << i << ":" << acc << ";";
    }
    std::cout << std::endl;
}

void ProbabilityEstimation::printResult() {
    /*if (options.getXmlTrace()) {
        printXMLTrace(m, printStack, query, tapn);
    } else {
        printHumanTrace(m, printStack, query->getQuantifier());
    }*/
    float result = getEstimation();
    float width = smcSettings.estimationIntervalWidth;
    std::cout << "Probability estimation:" << std::endl;
    std::cout << "\tConfidence: " << smcSettings.confidence * 100 << "%" << std::endl;
    std::cout << "\tP = " << result << " ± " << width << std::endl;
}

}
