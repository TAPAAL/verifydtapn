#include "DiscreteVerification/VerificationTypes/ProbabilityFloatComparison.hpp"
#include "DiscreteVerification/QueryVisitor.hpp"

#include <iostream>

namespace VerifyTAPN::DiscreteVerification {

ProbabilityFloatComparison::ProbabilityFloatComparison(
    TAPN::TimedArcPetriNet &tapn, NonStrictMarking &initialMarking, AST::Query *query, VerificationOptions options
)
: SMCVerification(tapn, initialMarking, query, options), validRuns(0), ratio(0)
{ 
    computeAcceptingBounds(options.getFalsePositives(), options.getFalseNegatives()); //TODO: Options
    computeIndifferenceRegion(options.getTargetProbability(), options.getIndifferenceUp(), options.getIndifferenceDown());
}

void ProbabilityFloatComparison::handleRunResult(const bool res) {
    bool valid = query->getQuantifier() == PG ? !res : res;
    ratio += valid ? log(p1 / p0) : log((1 - p1) / (1 - p0));
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
    if(ratio <= boundH0) {
        result = true;
        return false;
    } else if(ratio >= boundH1) {
        result = false;
        return false;
    }
    return true;
}

bool ProbabilityFloatComparison::getResult() {
    return result;
}

void ProbabilityFloatComparison::computeAcceptingBounds(const float alpha, const float beta) {
    boundH0 = log(beta / (1 - alpha));
    boundH1 = log((1 - beta) / alpha);
} 

void ProbabilityFloatComparison::computeIndifferenceRegion(const float p, const float sigma0, const float sigma1) {
    p0 = p + sigma0;
    p1 = p - sigma1;
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
    std::cout << "Probability comparison :" << std::endl;
    std::cout << "\tQuery : P >= " << options.getTargetProbability() << std::endl;
    std::cout << "\tIndifference region : [" << p1 << "," << p0 << "]" << std::endl;
    std::cout << "\tFalse positives : " << options.getFalsePositives() << std::endl;
    std::cout << "\tFalse negatives : " << options.getFalseNegatives() << std::endl;
    std::cout << "\tResult : " << result << std::endl;
}

}