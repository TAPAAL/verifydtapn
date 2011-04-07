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
#include "Core/SymbolicMarking/DiscreteInclusionMarkingFactory.hpp"

using namespace std;
using namespace VerifyTAPN;
using namespace VerifyTAPN::TAPN;
using namespace boost;

namespace VerifyTAPN{
	class MarkingFactory;
	class SymbolicMarking;
}

void print(const SymbolicMarking& marking){
		const DBMMarking& dbmMarking = static_cast<const DBMMarking&>(marking);
		std::cout << "id: " << dbmMarking.UniqueId() << "\n";
		std::cout << "dp: ";
		for(unsigned int i = 0; i < dbmMarking.NumberOfTokens(); i++){
			if(i != 0) std::cout << ", ";
			std::cout << dbmMarking.GetTokenPlacement(i);
		}
		std::cout << "\nmapping: ";
		for(unsigned int i = 0; i < dbmMarking.NumberOfTokens(); i++){
			if(i != 0) std::cout << ", ";
			std::cout << i << ":" << dbmMarking.GetClockIndex(i);
		}
		std::cout << "\ndbm:\n";
		std::cout << dbmMarking.GetDBM();
		std::cout << "\n";
	}

void test(){
	std::vector<int> vec;
	vec.push_back(4);
	vec.push_back(2);
	vec.push_back(1);
	vec.push_back(3);

	DiscretePart dp(vec);

	dbm::dbm_t dbm(5);
	dbm.setInit();
	dbm.constrain(0,1, -4, false);
	dbm.constrain(1,0, 8, false);
	dbm.constrain(0,2, -2, false);
	dbm.constrain(2,0, 4, false);
	dbm.constrain(0,3, -1, false);
	dbm.constrain(3,0, 2, false);
	dbm.constrain(0,4, -3, false);
	dbm.constrain(4,0, 6, false);

	DBMMarking marking(dp,dbm);

	print(marking);

	typedef boost::bimap<unsigned int, unsigned int> Map;
	Map map;
	marking.MakeSymmetric(map);

	print(marking);
	std::cout << "\n\nIndirection Table:\n";
	for(Map::const_iterator it = map.begin(); it != map.end(); it++)
	{
		 std::cout << it->left << " <--> " << it->right << std::endl;
	}
}

int main(int argc, char* argv[]) {
	//test(); return 0;
	VerificationOptions options = VerificationOptions::ParseVerificationOptions(argc, argv);

	TAPNXmlParser modelParser;
	boost::shared_ptr<TAPN::TimedArcPetriNet> tapn = modelParser.Parse(options.GetInputFile());
	tapn->Initialize(options.GetUntimedPlacesEnabled());

	MarkingFactory* factory = new UppaalDBMMarkingFactory(tapn);
	//MarkingFactory* factory = new DiscreteInclusionMarkingFactory(tapn);

	std::vector<int> initialPlacement(modelParser.ParseMarking(options.GetInputFile(), *tapn));
	SymbolicMarking* initialMarking(factory->InitialMarking(initialPlacement));

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


