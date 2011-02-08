#include <iostream>
#include "Core/TAPN/TAPN.hpp"
#include "boost/smart_ptr.hpp"
#include "Core/TAPNParser/TAPNXmlParser.hpp"
#include "Core/VerificationOptions.hpp"
#include "Core/SymbolicMarking/SymMarking.hpp"
#include "Core/QueryParser/AST.hpp"
#include "Core/QueryParser/TAPNQueryParser.hpp"
#include "ReachabilityChecker/SearchStrategy.hpp"
#include "dbm/print.h"
#include "Core/SymbolicMarking/DBMMarking.hpp"

using namespace std;
using namespace VerifyTAPN;
using namespace VerifyTAPN::TAPN;
using namespace boost;

void test(){
	DiscretePart dp;
	dbm::dbm_t dbm;
	DBMMarking marking(dp, dbm);
}

int main(int argc, char* argv[]) {
	//test();
	VerificationOptions options = VerificationOptions::ParseVerificationOptions(argc, argv);
	MarkingFactory* factory = NULL;

	TAPNXmlParser modelParser;
	boost::shared_ptr<TAPN::TimedArcPetriNet> tapn = modelParser.Parse(options.GetInputFile());
	tapn->Initialize(options.GetUntimedPlacesEnabled());
	SymMarking* initialMarking = modelParser.ParseMarking(options.GetInputFile(), *tapn);

	TAPNQueryParser queryParser(*tapn);
	queryParser.parse(options.QueryFile());
	AST::Query* query = queryParser.GetAST();

	SearchStrategy* strategy = new DefaultSearchStrategy(*tapn, NULL, query, options, factory);

	bool result = strategy->Verify();

	std::cout << strategy->GetStats() << std::endl;
	std::cout << "Query is " << (result ? "satisfied" : "NOT satisfied") << "." << std::endl;
	strategy->PrintTraceIfAny(result);
	delete strategy;

	return 0;
}


