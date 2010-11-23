#include <iostream>
#include "TAPN/TAPN.hpp"
#include "boost/smart_ptr.hpp"
#include "Core/TAPNXmlParser.hpp"
#include "Core/VerificationOptions.hpp"
#include "Core/SymMarking.hpp"
#include "Core/QueryParser/AST.hpp"
#include "Core/QueryParser/TAPNQueryParser.hpp"
#include "Verification/SearchStrategy.hpp"
#include "dbm/print.h"

using namespace std;
using namespace VerifyTAPN;
using namespace VerifyTAPN::TAPN;
using namespace boost;

void test()
{
	dbm::dbm_t dbm(3);

	dbm.setInit();
	dbm.constrain(0,1, dbm_bound2raw(-1, dbm_WEAK));
	dbm.constrain(1,0,dbm_bound2raw(3,dbm_WEAK));
	dbm.constrain(0,2,dbm_bound2raw(-1,dbm_WEAK));
	dbm.constrain(2,0,dbm_bound2raw(5,dbm_WEAK));
	dbm.constrain(1,2,dbm_bound2raw(2,dbm_WEAK));
	dbm.constrain(2,1,dbm_bound2raw(2, dbm_WEAK));


	int max[] = { 3,-dbm_INFINITY,3 };

	std::cout << "dbm: \n";
	std::cout << dbm << "\n\n";

	dbm::dbm_t dbm2(3);

	dbm2.setInit();
	dbm2.constrain(0,1, dbm_bound2raw(-2, dbm_WEAK));
	dbm2.constrain(1,0,dbm_bound2raw(3,dbm_WEAK));
	dbm2.constrain(0,2,dbm_bound2raw(-1,dbm_WEAK));
	dbm2.constrain(2,0,dbm_bound2raw(4,dbm_WEAK));
	dbm2.constrain(1,2,dbm_bound2raw(2,dbm_WEAK));
	dbm2.constrain(2,1,dbm_bound2raw(2, dbm_WEAK));


	std::cout << "dbm2: \n";
	std::cout << dbm2 << "\n\n";

	relation_t relation = dbm2.relation(dbm);

	std::cout << "relation:\n";
	std::cout << "dbm2 <subset eq> dbm: " << ((relation & base_SUBSET) != 0 ? "true" : "false") << "\n";

	dbm2.extrapolateMaxBounds(max);

	std::cout << "dbm2 inactive:\n";
	std::cout << dbm2 << "\n";

	relation = dbm2.relation(dbm);

	std::cout << "relation:\n";
	std::cout << "dbm2 <subset eq> dbm: " << ((relation & base_SUBSET) != 0 ? "true" : "false") << "\n";


}

int main(int argc, char* argv[]) {
	//test();
	VerificationOptions options = VerificationOptions::ParseVerificationOptions(argc, argv);

	TAPNXmlParser modelParser;
	boost::shared_ptr<TAPN::TimedArcPetriNet> tapn = modelParser.Parse(options.GetInputFile());
	tapn->Initialize(options.GetInfinityPlacesEnabled());
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
