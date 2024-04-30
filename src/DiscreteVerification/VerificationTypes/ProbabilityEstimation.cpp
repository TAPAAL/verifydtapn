#include "DiscreteVerification/VerificationTypes/ProbabilityEstimation.hpp"
#include "DiscreteVerification/QueryVisitor.hpp"

#include <math.h>

namespace VerifyTAPN::DiscreteVerification {

ProbabilityEstimation::ProbabilityEstimation(
    TAPN::TimedArcPetriNet &tapn, NonStrictMarking &initialMarking, AST::Query *query, VerificationOptions options
)
: SMCVerification(tapn, initialMarking, query, options), validRuns(0)
{
    computeChernoffHoeffdingBound(options.getSmcIntervalWidth(), options.getSmcConfidence());
}

bool ProbabilityEstimation::mustDoAnotherRun() {
    return numberOfRuns < runsNeeded;
}

void ProbabilityEstimation::handleRunResult(const bool res) {
    if(query->getQuantifier() == PG) validRuns += res ? 0 : 1;
    else validRuns += res ? 1 : 0;
}

bool ProbabilityEstimation::handleSuccessor(NonStrictMarking* marking) {
    marking->cut(placeStats);

    QueryVisitor<NonStrictMarking> checker(*marking, tapn);
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
}

void ProbabilityEstimation::printResult() {
    /*if (options.getXmlTrace()) {
        printXMLTrace(m, printStack, query, tapn);
    } else {
        printHumanTrace(m, printStack, query->getQuantifier());
    }*/
    float result = getEstimation();
    float width = options.getSmcIntervalWidth();
    float minBound = std::max(result - width, 0.0f);
    float maxBound = std::min(result + width, 1.0f);
    std::cout << "Probability estimation :" << std::endl;
    std::cout << "\tConfidence : " << options.getSmcConfidence() * 100 << "%" << std::endl;
    std::cout << "\tP in [" << minBound << ";" << maxBound << "]" << std::endl;
}

}