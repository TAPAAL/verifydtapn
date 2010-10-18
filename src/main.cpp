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
#include "Verification/SuccessorGenerator.hpp"
#include "boost/numeric/ublas/matrix.hpp"
#include "boost/numeric/ublas/io.hpp"
#include "dbm/print.h"
#include "base/bitstring.h"

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

//void testIsEnabled(const string& filename)
//{
//	TAPNXmlParser *parser = new TAPNXmlParser();
//
//	boost::shared_ptr<TimedArcPetriNet> tapn = parser->Parse(filename);
//	tapn->Initialize();
//	boost::shared_ptr<SymMarking> marking = parser->ParseMarking(filename, *tapn);
//
//	for(TimedTransition::Vector::const_iterator iter = tapn->GetTransitions().begin(); iter != tapn->GetTransitions().end(); ++iter)
//	{
//		bool isEnabled = (*iter)->isEnabledBy(*tapn,*marking);
//
//		std::cout << (*(*iter)) << " enabled: " << isEnabled << "\n";
//	}
//}

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

void testSuccessorGenerator(const string& filename)
{
	TAPNXmlParser *parser = new TAPNXmlParser();

	boost::shared_ptr<TimedArcPetriNet> tapn = parser->Parse(filename);

    tapn->Initialize();

	boost::shared_ptr<SymMarking> marking = parser->ParseMarking(filename, *tapn);

	SuccessorGenerator succGen(tapn, 10);

	std::vector<SymMarking*> succVec(3);

	succGen.GenerateDiscreteTransitionsSuccessors(marking.get(),succVec);

	std::cout << "\n\nSuccessor Generation:\n";
	std::cout << "-----------------------------\n";

	std::cout << succGen << "\n";
}

void test()
{
	int size = 4;
	dbm::dbm_t dbm(size);
	dbm.setInit();

	dbm.constrain(0,1, dbm_bound2raw(-3,dbm_WEAK));
	dbm.constrain(1,0,dbm_bound2raw(7,dbm_WEAK));
	dbm.constrain(0,2, dbm_bound2raw(-1,dbm_STRICT));
	dbm.constrain(2,0,dbm_bound2raw(5, dbm_STRICT));

	int newDimension = dbm.getDimension()+3;
	dbm::dbm_t newDbm(newDimension);
	newDbm.setInit();

	unsigned int bitSrc = 0;
	unsigned int bitDst = 0;
	int bitval = 1;
	int dim = 0;
	unsigned int table[newDimension];
	while(dim < newDimension)
	{
		if(dim < dbm.getDimension())
			bitSrc |= bitval;

		bitDst |= bitval;
		bitval <<= 1;
		table[dim] = dim;
		dim++;
	}

	//std::cout << base_countBitsN(&bitSrc, 1) << "\n";

	dbm_shrinkExpand(dbm.getDBM(), newDbm.getDBM(), dbm.getDimension(), &bitSrc, &bitDst, 1, table);

	std::cout << dbm << "\n\n";
	std::cout << newDbm << std::endl;
}

int main(int argc, char* argv[]) {
	string filename = "/home/lassejac/Documents/Uni/development/DAT7/verifytapn/example-nets/simple_net2.xml";
	test();
	//testInitMarkingParsing(filename);

  //  testIsEnabled(filename);

	//testPairing(filename);
	//testSuccessorGenerator(filename);

    return 0;
}
