#include <iostream>
#include "Core/TAPN/TAPN.hpp"
#include "boost/smart_ptr.hpp"
#include "Core/TAPNParser/TAPNXmlParser.hpp"
#include "Core/VerificationOptions.hpp"
#include "Core/QueryParser/AST.hpp"
#include "Core/QueryParser/TAPNQueryParser.hpp"
#include "ReachabilityChecker/SearchStrategy.hpp"

#include "Core/SymbolicMarking/UppaalDBMMarkingFactory.hpp"
#include "Core/SymbolicMarking/DiscreteInclusionMarkingFactory.hpp"

#include "ReachabilityChecker/Trace/trace_exception.hpp"

using namespace std;
using namespace VerifyTAPN;
using namespace VerifyTAPN::TAPN;
using namespace boost;

MarkingFactory* CreateFactory(const VerificationOptions& options, const boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn)
{
	switch(options.GetFactory())
	{
	case DISCRETE_INCLUSION:
		return new DiscreteInclusionMarkingFactory(tapn);
	default:
		return new UppaalDBMMarkingFactory(tapn);
	}
}

int main(int argc, char* argv[])
{
	VerificationOptions options = VerificationOptions::ParseVerificationOptions(argc, argv);

	TAPNXmlParser modelParser;
	boost::shared_ptr<TAPN::TimedArcPetriNet> tapn;

	try{
		tapn = modelParser.Parse(options.GetInputFile());
	}catch(const std::string& e){
		std::cout << "There was an error parsing the model file: " << e << std::endl;
		return 1;
	}

	tapn->Initialize(options.GetUntimedPlacesEnabled());
	MarkingFactory* factory = CreateFactory(options, tapn);

	std::vector<int> initialPlacement(modelParser.ParseMarking(options.GetInputFile(), *tapn));
	SymbolicMarking* initialMarking(factory->InitialMarking(initialPlacement));

	AST::Query* query;
	try{
		TAPNQueryParser queryParser(*tapn);
		queryParser.parse(options.QueryFile());
		query = queryParser.GetAST();
	}catch(...){
		std::cout << "There was an error parsing the query file." << std::endl;
		return 1;
	}
	SearchStrategy* strategy = new DefaultSearchStrategy(*tapn, initialMarking, query, options, factory);

	bool result = strategy->Verify();
	std::cout << strategy->GetStats() << std::endl;
	std::cout << "Query is " << (result ? "satisfied" : "NOT satisfied") << "." << std::endl;

	try{
		strategy->PrintTraceIfAny(result);
	}catch(const trace_exception& e){
		std::cout << "There was an error generating a trace. This is a bug. Please report this on launchpad and attach your TAPN model and this error message: ";
		std::cout << e.what() << std::endl;
		return 1;
	}
	delete strategy;
	delete factory;

	return 0;
}


