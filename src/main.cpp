#include <iostream>
#include "Core/TAPN/TAPN.hpp"
#include "boost/smart_ptr.hpp"
#include "Core/TAPNParser/TAPNXmlParser.hpp"
#include "Core/VerificationOptions.hpp"
#include "Core/QueryParser/AST.hpp"
#include "Core/QueryParser/TAPNQueryParser.hpp"
#include "ReachabilityChecker/SearchStrategy.hpp"
#include "dbm/print.h"

#include "Core/SymbolicMarking/UppaalDBMMarkingFactory.hpp"

using namespace std;
using namespace VerifyTAPN;
using namespace VerifyTAPN::TAPN;
using namespace boost;

namespace VerifyTAPN{
	class MarkingFactory;
	class SymbolicMarking;
}

int main(int argc, char* argv[]) {
	VerificationOptions options = VerificationOptions::ParseVerificationOptions(argc, argv);
	MarkingFactory* factory = new UppaalDBMMarkingFactory();
	DBMMarking::factory = factory;
	TAPNXmlParser modelParser(factory);
	boost::shared_ptr<TAPN::TimedArcPetriNet> tapn = modelParser.Parse(options.GetInputFile());
	tapn->Initialize(options.GetUntimedPlacesEnabled());
	SymbolicMarking* initialMarking = modelParser.ParseMarking(options.GetInputFile(), *tapn);

	TAPNQueryParser queryParser(*tapn);
	queryParser.parse(options.QueryFile());
	AST::Query* query = queryParser.GetAST();

	SearchStrategy* strategy = new DefaultSearchStrategy(*tapn, initialMarking, query, options, factory);

	bool result = strategy->Verify();

	std::cout << strategy->GetStats() << std::endl;
	std::cout << "Query is " << (result ? "satisfied" : "NOT satisfied") << "." << std::endl;
	strategy->PrintTraceIfAny(result);

	delete strategy;
	delete factory;

	return 0;
}


