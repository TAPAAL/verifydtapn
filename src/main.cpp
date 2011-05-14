#include <iostream>
#include "Core/TAPN/TAPN.hpp"
#include "boost/smart_ptr.hpp"
#include "Core/TAPNParser/TAPNXmlParser.hpp"
#include "Core/VerificationOptions.hpp"
#include "Core/ArgsParser.hpp"
#include "Core/QueryParser/AST.hpp"
#include "Core/QueryParser/UpwardClosedVisitor.hpp"
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
	case OLD_FACTORY:
		return new UppaalDBMMarkingFactory(tapn);
	default:// Note that the constructor of DiscreteInclusionMarkingFactory automatically disables discrete inclusion
		    // if DEFAULT is chosen
		return new DiscreteInclusionMarkingFactory(tapn, options);
	};
}

int main(int argc, char* argv[])
{
	ArgsParser parser;
	VerificationOptions options = parser.Parse(argc, argv);

	TAPNXmlParser modelParser;
	boost::shared_ptr<TAPN::TimedArcPetriNet> tapn;

	try{
		tapn = modelParser.Parse(options.GetInputFile());
	}catch(const std::string& e){
		std::cout << "There was an error parsing the model file: " << e << std::endl;
		return 1;
	}
	tapn->Initialize(options.GetUntimedPlacesEnabled());
	std::vector<int> initialPlacement(modelParser.ParseMarking(options.GetInputFile(), *tapn));

	AST::Query* query;
	try{
		TAPNQueryParser queryParser(*tapn);
		queryParser.parse(options.QueryFile());
		query = queryParser.GetAST();
	}catch(...){
		std::cout << "There was an error parsing the query file." << std::endl;
		return 1;
	}

	if(options.GetFactory() == DISCRETE_INCLUSION)
	{
		AST::UpwardClosedVisitor visitor;
		bool upward_closed = visitor.IsUpwardClosed(*query);
		if(!upward_closed)
		{
			options.SetFactory(DEFAULT);
			std::cout << "** The specified query is not upward closed. Disabling discrete inclusion optimization." << std::endl;
		}
	}

	MarkingFactory* factory = CreateFactory(options, tapn);
	SymbolicMarking* initialMarking(factory->InitialMarking(initialPlacement));
	if(initialMarking->NumberOfTokens() > options.GetKBound())
	{
		std::cout << "The specified k-bound is less than the number of tokens in the initial markings.";
		return 1;
	}

	SearchStrategy* strategy = new DefaultSearchStrategy(*tapn, initialMarking, query, options, factory);

	std::cout << options << std::endl;
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


