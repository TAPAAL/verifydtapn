#include "DiscreteVerification/VerificationTypes/SMCTracesGenerator.hpp"
#include "DiscreteVerification/QueryVisitor.hpp"

#include <math.h>

namespace VerifyTAPN::DiscreteVerification {

SMCTracesGenerator::SMCTracesGenerator(
    TAPN::TimedArcPetriNet &tapn, RealMarking &initialMarking, AST::SMCQuery *query, VerificationOptions options
)
: SMCVerification(tapn, initialMarking, query, options)
{
}

bool SMCTracesGenerator::mustDoAnotherRun() {
    return mustSaveTrace();
}

void SMCTracesGenerator::handleRunResult(const bool res, int steps, double delay)
{
    
}

bool SMCTracesGenerator::handleSuccessor(RealMarking* marking) {
    QueryVisitor<RealMarking> checker(*marking, tapn);
    AST::BoolResult context;
    query->accept(checker, context);
    delete marking;
    return context.value;
}

void SMCTracesGenerator::printResult() {
    std::cout << "Generated " << traces.size() << " random traces" << std::endl;
}

}
