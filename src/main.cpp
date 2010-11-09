#include <iostream>
#include "TAPN/TAPN.hpp"
#include "boost/smart_ptr.hpp"
#include "Core/TAPNXmlParser.hpp"
#include "Core/VerificationOptions.hpp"
#include "Core/SymMarking.hpp"
#include "Core/QueryParser/AST.hpp"
#include "Core/QueryParser/TAPNQueryParser.hpp"
#include "Verification/SearchStrategy.hpp"

using namespace std;
using namespace VerifyTAPN;
using namespace VerifyTAPN::TAPN;
using namespace boost;



int main(int argc, char* argv[]) {
	VerificationOptions options = VerificationOptions::ParseVerificationOptions(argc, argv);

	TAPNXmlParser modelParser;
	boost::shared_ptr<TAPN::TimedArcPetriNet> tapn = modelParser.Parse(options.GetInputFile());
	tapn->Initialize();
	SymMarking* initialMarking = modelParser.ParseMarking(options.GetInputFile(), *tapn);


	TAPNQueryParser queryParser(*tapn);
	queryParser.parse(options.QueryFile());
	AST::Query* query = queryParser.GetAST();

	SearchStrategy* strategy = NULL;

	strategy = new DFS(*tapn, initialMarking, query, options);

	bool result = strategy->Verify();

	std::cout << strategy->GetStats() << std::endl;
	std::cout << "Query is " << (result ? "satisfied" : "NOT satisfied") << "." << std::endl;

	delete strategy;

	return 0;
}
