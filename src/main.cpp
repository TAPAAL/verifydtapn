#include <iostream>
#include "Core/TAPNParser/TAPNXmlParser.hpp"
#include "Core/VerificationOptions.hpp"
#include "Core/ArgsParser.hpp"
#include "Core/QueryParser/TAPNQueryParser.hpp"
#include "Core/TAPN/TimedPlace.hpp"
#include "DiscreteVerification/DiscreteVerification.hpp"
#include "DiscreteVerification/DeadlockVisitor.hpp"

using namespace std;
using namespace VerifyTAPN;
using namespace VerifyTAPN::TAPN;

int main(int argc, char *argv[]) {
    srand(time(nullptr));

    ArgsParser parser;
    VerificationOptions options = parser.parse(argc, argv);

    TAPNXmlParser modelParser(options.getReplacements());
    TAPN::TimedArcPetriNet *tapn;

    try {
        tapn = modelParser.parse(options.getInputFile());
    } catch (const std::string &e) {
        std::cout << "There was an error parsing the model file: " << e << std::endl;
        return 1;
    }

    tapn->initialize(options.getGlobalMaxConstantsEnabled(), options.getGCDLowerGuardsEnabled());

    if (options.getCalculateCmax()) {
        std::cout << options << std::endl;
        std::cout << "C-MAX" << std::endl;
        for (TimedPlace::Vector::const_iterator iter = tapn->getPlaces().begin();
             iter != tapn->getPlaces().end(); ++iter) {
            std::cout << '<' << (*iter)->getName() << '#' << (*iter)->getMaxConstant() << "> ";
        }
        std::cout << std::endl;
        return 0;
    }

    std::vector<int> initialPlacement(modelParser.parseMarking(options.getInputFile(), *tapn));

    AST::Query *query = nullptr;
    if (options.getWorkflowMode() == VerificationOptions::WORKFLOW_SOUNDNESS ||
        options.getWorkflowMode() == VerificationOptions::WORKFLOW_STRONG_SOUNDNESS) {
        if (options.getGCDLowerGuardsEnabled()) {
            cout << "Workflow-analysis does not support GCD-lowering" << endl;
            exit(1);
        }

        if (options.getSearchType() != VerificationOptions::DEFAULT) {
            cout << "Workflow-analysis only supports the default search-strategy" << endl;
            exit(1);
        }

        if (options.getQueryFile() != "") {
            cout << "Workflow-analysis does not accept a query file" << endl;
            exit(1);
        }

        if (options.getWorkflowMode() == VerificationOptions::WORKFLOW_SOUNDNESS) {
            options.setSearchType(VerificationOptions::MINDELAYFIRST);
            query = new AST::Query(AST::EF, new AST::BoolExpression(true));
        } else if (options.getWorkflowMode() == VerificationOptions::WORKFLOW_STRONG_SOUNDNESS) {
            options.setSearchType(VerificationOptions::DEPTHFIRST);
            query = new AST::Query(AST::AF, new AST::BoolExpression(false));
        }

    } else {
        try {
            TAPNQueryParser queryParser(*tapn);
            queryParser.parse(options.getQueryFile());
            query = queryParser.getAST();
        } catch (...) {
            std::cout << "There was an error parsing the query file." << std::endl;
            return 1;
        }

        if (options.getTrace() != VerificationOptions::NO_TRACE &&
            (query->getQuantifier() == AST::CF || query->getQuantifier() == AST::CG)) {
            std::cout << "Traces are not supported for game-synthesis" << std::endl;
            return 1;
        }

        if (options.getTrace() == VerificationOptions::FASTEST_TRACE &&
            (options.getSearchType() != VerificationOptions::DEFAULT ||
             query->getQuantifier() == AST::EG || query->getQuantifier() == AST::AF ||
             options.getVerificationType() == VerificationOptions::TIMEDART)) {
            std::cout
                    << "Fastest trace-option is only supported for reachability queries with default search strategy and without time darts."
                    << std::endl;
            return 1;
        } else if (options.getTrace() == VerificationOptions::FASTEST_TRACE) {
            options.setSearchType(VerificationOptions::MINDELAYFIRST);
        } else if (options.getSearchType() == VerificationOptions::DEFAULT) {
            options.setSearchType(VerificationOptions::COVERMOST);
        }
    }

    if (tapn->containsOrphanTransitions()) {
        std::cout << "The model contains orphan transitions. This is not supported by the engine." << std::endl;
        return 1;
    }

    if (query->getQuantifier() == AST::CF || query->getQuantifier() == AST::CG) {
        options.setKeepDeadTokens(true);
    }

    tapn->updatePlaceTypes(query, options);

    int result = DiscreteVerification::DiscreteVerification::run(*tapn, initialPlacement, query, options);

    // cleanup
    delete tapn;
    delete query;

    // return result
    return result;
}


