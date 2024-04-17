#include "DiscreteVerification/VerificationTypes/ProbabilityFloatComparison.hpp"
#include "DiscreteVerification/QueryVisitor.hpp"

#include <iostream>

namespace VerifyTAPN::DiscreteVerification {

ProbabilityFloatComparison::ProbabilityFloatComparison(
    TAPN::TimedArcPetriNet &tapn, NonStrictMarking &initialMarking, AST::Query *query, VerificationOptions options
)
: SMCVerification(tapn, initialMarking, query, options), validRuns(0)
{
    computeAcceptingBounds(0.5, 0.5); //TODO: Options
}

void ProbabilityFloatComparison::handleRunResult(const bool res) {
    if(query->getQuantifier() == PG) validRuns += res ? 0 : 1;
    else validRuns += res ? 1 : 0;
}

bool ProbabilityFloatComparison::handleSuccessor(NonStrictMarking* marking) {
    marking->cut(placeStats);

    QueryVisitor<NonStrictMarking> checker(*marking, tapn);
    AST::BoolResult context;
    query->accept(checker, context);

    delete marking;

    return context.value;
}

bool ProbabilityFloatComparison::mustDoAnotherRun() {
    return false;
}

bool ProbabilityFloatComparison::getResult() {
    return false;
}

void ProbabilityFloatComparison::computeAcceptingBounds(const float alpha, const float beta) {

} 

void ProbabilityFloatComparison::printStats() {
    SMCVerification::printStats();
    std::cout << "  valid runs:\t" << validRuns << std::endl;
}

void ProbabilityFloatComparison::printResult() {
    /*if (options.getXmlTrace()) {
        printXMLTrace(m, printStack, query, tapn);
    } else {
        printHumanTrace(m, printStack, query->getQuantifier());
    }*/
    bool result = getResult();
    float width = options.getSmcIntervalWidth();
    float minBound = std::max(result - width, 0.0f);
    float maxBound = std::min(result + width, 1.0f);
    std::cout << "Probability comparison :" << std::endl;
    std::cout << "\tConfidence : " << (1 - options.getSmcConfidence()) << "%" << std::endl;
}

}