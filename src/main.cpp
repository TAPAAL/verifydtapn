#include <iostream>
#include "boost/smart_ptr.hpp"
#include "Core/TAPNParser/TAPNXmlParser.hpp"
#include "Core/VerificationOptions.hpp"
#include "Core/ArgsParser.hpp"
#include "Core/QueryParser/UpwardClosedVisitor.hpp"
#include "Core/QueryParser/TAPNQueryParser.hpp"
#include "Core/QueryParser/NormalizationVisitor.hpp"
#include "Core/QueryParser/ToStringVisitor.hpp"
#include "Core/QueryParser/BadPlaceVisitor.hpp"
#include "Core/TAPN/TimedPlace.hpp"

#include "ReachabilityChecker/Search/SearchStrategy.hpp"
#include "ReachabilityChecker/Search/BFS.hpp"
#include "ReachabilityChecker/Search/DFS.hpp"
#include "ReachabilityChecker/Search/CoverMostSearch.hpp"
#include "ReachabilityChecker/Search/RandomSearch.hpp"

#include "Core/SymbolicMarking/UppaalDBMMarkingFactory.hpp"
#include "Core/SymbolicMarking/DiscreteInclusionMarkingFactory.hpp"

#include "ReachabilityChecker/Trace/trace_exception.hpp"

#include "DiscreteVerification/DiscreteVerification.hpp"

using namespace std;
using namespace VerifyTAPN;
using namespace VerifyTAPN::TAPN;
using namespace boost;

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

	tapn->Initialize(options.GetUntimedPlacesEnabled(), options.GetGlobalMaxConstantsEnabled());

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

	if(query->GetQuantifier() == AST::EF || query->GetQuantifier() == AST::AG){
		tapn->removeOrphantedTransitions();
	}

	tapn->updatePlaceTypes(query);

	return DiscreteVerification::DiscreteVerification::run(tapn, initialPlacement, query, options);
}


