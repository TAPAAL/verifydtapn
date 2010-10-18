#include <iostream>
#include "TAPN/TAPN.hpp"
#include "boost/smart_ptr.hpp"
#include "boost/make_shared.hpp"
#include "Core/TAPNXmlParser.hpp"
#include "Core/VerificationOptions.hpp"
#include "Core/DiscretePart.hpp"
#include "Core/SymMarking.hpp"
#include "Verification/PWList.hpp"
#include "Core/Pairing.hpp"
#include "Core/QueryParser/AST.hpp"
#include "Core/QueryParser/TAPNQueryParser.hpp"
#include "Core/QueryParser/ToStringVisitor.hpp"
#include "Core/QueryChecker.hpp"
#include <vector>

using namespace std;
using namespace VerifyTAPN;
using namespace VerifyTAPN::TAPN;
using namespace boost;

void testInitMarkingParsing(const string& filename)
{
    TAPNXmlParser *parser = new TAPNXmlParser();

    boost::shared_ptr<TimedArcPetriNet> tapn = parser->Parse(filename);
    boost::shared_ptr<SymMarking> marking = parser->ParseMarking(filename, *tapn);

    std::cout << *tapn << "\n\n\n";

    std::cout << "Initial Marking:" << "\n";
    std::cout << "-------------------------------" << "\n";
    std::cout << "\nPlacement Vector:\n";

    int i = 0;
    for(std::vector<int>::const_iterator iter = marking->GetDiscretePart()->GetTokenPlacementVector().begin();iter != marking->GetDiscretePart()->GetTokenPlacementVector().end();++iter){
        std::cout << i << ":" << (*iter) << "\n";
        i++;
    }
    std::cout << "\n\nMapping Vector:\n";
    i = 0;
    for(std::vector<int>::const_iterator iter2 = marking->GetTokenMapping().GetMappingVector().begin();iter2 != marking->GetTokenMapping().GetMappingVector().end();++iter2){
        std::cout << i << ":" << (*iter2) << "\n";
        i++;
    }
}

void testIsEnabled(const string& filename)
{
	TAPNXmlParser *parser = new TAPNXmlParser();

	boost::shared_ptr<TimedArcPetriNet> tapn = parser->Parse(filename);
	tapn->Initialize();
	boost::shared_ptr<SymMarking> marking = parser->ParseMarking(filename, *tapn);

	for(TimedTransition::Vector::const_iterator iter = tapn->GetTransitions().begin(); iter != tapn->GetTransitions().end(); ++iter)
	{
		bool isEnabled = (*iter)->isEnabledBy(*tapn,*marking);

		std::cout << (*(*iter)) << " enabled: " << isEnabled << "\n";
	}
}

void testPairing(const string& filename)
{
	TAPNXmlParser *parser = new TAPNXmlParser();

	boost::shared_ptr<TimedArcPetriNet> tapn = parser->Parse(filename);
	tapn->Initialize();
	boost::shared_ptr<SymMarking> marking = parser->ParseMarking(filename, *tapn);


	for(TimedTransition::Vector::const_iterator iter = tapn->GetTransitions().begin(); iter != tapn->GetTransitions().end(); ++iter)
	{
		Pairing pair(*(*iter));

		std::cout << "Transition " << (*iter)->GetName() << " pairing:" << endl;

		std::cout << pair;
	}
}

int main(int argc, char* argv[]) {
	//string filename = "example-nets/pairing_test_net.xml";
	//testInitMarkingParsing(filename);

  //  testIsEnabled(filename);

	//testPairing(filename);

	string file = "example-nets/simple_net.xml";
	TAPNXmlParser parser;
	boost::shared_ptr<TimedArcPetriNet> tapn = parser.Parse(file);

	VerifyTAPN::TAPNQueryParser queryParser(*tapn);
	queryParser.parse("example-queries/ef-query-test");
	AST::Query* ast = queryParser.GetAST();
	VerifyTAPN::AST::ToStringVisitor visitor;
	boost::any any;
	ast->Accept(visitor, any);

	dbm::dbm_t dbm;
	std::vector<int> vec1;
	vec1.push_back(1);
	vec1.push_back(1);
//	vec1.push_back(1);
	vec1.push_back(2);
	vec1.push_back(3);

	DiscretePart dp1(vec1);
	SymMarking marking(dp1, dbm);

	QueryChecker checker(ast);
	std::cout << std::endl << checker.IsExpressionSatisfied(marking);


    return 0;
}
