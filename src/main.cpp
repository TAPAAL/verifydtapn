#include <iostream>
#include "boost/smart_ptr.hpp"
#include "Core/TAPNParser/TAPNXmlParser.hpp"
#include "Core/VerificationOptions.hpp"
#include "Core/ArgsParser.hpp"
#include "Core/QueryParser/UpwardClosedVisitor.hpp"
#include "Core/QueryParser/TAPNQueryParser.hpp"

#include "ReachabilityChecker/Search/SearchStrategy.hpp"
#include "ReachabilityChecker/Search/BFS.hpp"
#include "ReachabilityChecker/Search/DFS.hpp"
#include "ReachabilityChecker/Search/CoverMostSearch.hpp"
#include "ReachabilityChecker/Search/RandomSearch.hpp"

#include "Core/SymbolicMarking/UppaalDBMMarkingFactory.hpp"
#include "Core/SymbolicMarking/DiscreteInclusionMarkingFactory.hpp"

#include "ReachabilityChecker/Trace/trace_exception.hpp"
//#include "Core/QueryParser/ToStringVisitor.hpp"
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

SearchStrategy* CreateSearchStrategy(const boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn, SymbolicMarking* initialMarking, AST::Query* query, const VerificationOptions& options, MarkingFactory* factory)
{
	SearchStrategy* strategy;

	switch(options.GetSearchType())
	{
	case DEPTHFIRST:
		strategy = new DFS(*tapn, initialMarking, query, options, factory);
		break;
	case COVERMOST:
		if(options.GetFactory() == DISCRETE_INCLUSION)
			strategy = new CoverMostSearch(*tapn, initialMarking, query, options, factory);
		else
			strategy = new BFS(*tapn, initialMarking, query, options, factory);
		break;
	case RANDOM:
		strategy = new RandomSearch(*tapn, initialMarking, query, options, factory);
		break;
	default:
		strategy = new BFS(*tapn, initialMarking, query, options, factory);
		break;
	}
	strategy->Init();
	return strategy;
}

int main(int argc, char* argv[])
{
	srand ( time(NULL) );

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
			std::cout << "The specified query is not upward closed and is therefore not supported by the discrete inclusion optimization." << std::endl;
			return 1;
		}
	}

	MarkingFactory* factory = CreateFactory(options, tapn);
	SymbolicMarking* initialMarking(factory->InitialMarking(initialPlacement));
	if(initialMarking->NumberOfTokens() > options.GetKBound())
	{
		std::cout << "The specified k-bound is less than the number of tokens in the initial markings.";
		return 1;
	}

	SearchStrategy* strategy = CreateSearchStrategy(tapn, initialMarking, query, options, factory);

	std::cout << options << std::endl;
	bool result = strategy->Verify();
	std::cout << strategy->GetStats() << std::endl;
	std::cout << "Query is " << (result ? "satisfied" : "NOT satisfied") << "." << std::endl;
	std::cout << "Max number of tokens found in any reachable marking: ";
	if(strategy->MaxUsedTokens() == options.GetKBound() + 1)
		std::cout << ">" << options.GetKBound() << std::endl;
	else
		std::cout << strategy->MaxUsedTokens() << std::endl;

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


