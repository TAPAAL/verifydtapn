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

//void testInitMarkingParsing(const string& filename)
//{
//    TAPNXmlParser *parser = new TAPNXmlParser();
//
//    boost::shared_ptr<TimedArcPetriNet> tapn = parser->Parse(filename);
//    boost::shared_ptr<SymMarking> marking = parser->ParseMarking(filename, *tapn);
//
//    std::cout << *tapn << "\n\n\n";
//    std::cout << *marking << "\n\n\n";
//
////    std::cout << "Initial Marking:" << "\n";
////    std::cout << "-------------------------------" << "\n";
////    std::cout << "\nPlacement Vector:\n";
////
////    int i = 0;
////    for(std::vector<int>::const_iterator iter = marking->GetDiscretePart()->GetTokenPlacementVector().begin();iter != marking->GetDiscretePart()->GetTokenPlacementVector().end();++iter){
////        std::cout << i << ":" << (*iter) << "\n";
////        i++;
////    }
////    std::cout << "\n\nMapping Vector:\n";
////    i = 0;
////    for(std::vector<int>::const_iterator iter2 = marking->GetTokenMapping().GetMappingVector().begin();iter2 != marking->GetTokenMapping().GetMappingVector().end();++iter2){
////        std::cout << i << ":" << (*iter2) << "\n";
////        i++;
////    }
//}
//
////void testIsEnabled(const string& filename)
////{
////	TAPNXmlParser *parser = new TAPNXmlParser();
////
////	boost::shared_ptr<TimedArcPetriNet> tapn = parser->Parse(filename);
////	tapn->Initialize();
////	boost::shared_ptr<SymMarking> marking = parser->ParseMarking(filename, *tapn);
////
////	for(TimedTransition::Vector::const_iterator iter = tapn->GetTransitions().begin(); iter != tapn->GetTransitions().end(); ++iter)
////	{
////		bool isEnabled = (*iter)->isEnabledBy(*tapn,*marking);
////
////		std::cout << (*(*iter)) << " enabled: " << isEnabled << "\n";
////	}
////}
//
//void testPairing(const string& filename)
//{
//	TAPNXmlParser *parser = new TAPNXmlParser();
//
//	boost::shared_ptr<TimedArcPetriNet> tapn = parser->Parse(filename);
//
//    tapn->Initialize();
//
//	boost::shared_ptr<SymMarking> marking = parser->ParseMarking(filename, *tapn);
//
//	for(TimedTransition::Vector::const_iterator iter = tapn->GetTransitions().begin(); iter != tapn->GetTransitions().end(); ++iter)
//	{
//		Pairing pair(*(*iter));
//
//		std::cout << "Transition " << (*iter)->GetName() << " pairing:" << endl;
//
//		std::cout << pair;
//	}
//}
//
//void testSuccessorGenerator(const string& filename)
//{
//	TAPNXmlParser *parser = new TAPNXmlParser();
//
//	boost::shared_ptr<TimedArcPetriNet> tapn = parser->Parse(filename);
//
//    tapn->Initialize();
//
//	boost::shared_ptr<SymMarking> marking = parser->ParseMarking(filename, *tapn);
//
//	// make marking k-bounded
//	marking->MakeKBound(4);
//
//	std::cout << *tapn << "\n\n\n";
//	std::cout << *marking << "\n\n\n";
//
//	SuccessorGenerator succGen(*tapn, 10);
//
//	marking->Delay();
//
//	std::cout << "Delay:\n";
//	std::cout << *marking << "\n\n\n";
//
//	std::vector<SymMarking*> succVec;
//
//	succGen.GenerateDiscreteTransitionsSuccessors(marking.get(),succVec);
//
//	std::cout << "\n\nSuccessor Generation:\n";
//	std::cout << "-----------------------------\n";
//
//	std::cout << succGen << "\n";
//
//	std::cout << "\n\nSuccessors: \n";
//	for(std::vector<SymMarking*>::const_iterator iter = succVec.begin(); iter != succVec.end(); ++iter)
//	{
//		std::cout << *(*iter) << "\n";
//	}
//
//}
//
//void test()
//{
//	int size = 40;
//	dbm::dbm_t dbm(size);
//	dbm.setInit();
//
//	dbm.constrain(0,1, dbm_bound2raw(-3,dbm_WEAK));
//	dbm.constrain(1,0,dbm_bound2raw(7,dbm_WEAK));
//	dbm.constrain(0,2, dbm_bound2raw(-1,dbm_STRICT));
//	dbm.constrain(2,0,dbm_bound2raw(5, dbm_STRICT));
//
//
//	unsigned int nAdditionalTokens = 24;
//	unsigned int oldDimension = dbm.getDimension();
//	unsigned int newDimension = oldDimension + nAdditionalTokens;
//
//	unsigned int bitArraySize = (newDimension % 32 == 0 ? newDimension/32 : newDimension/32+1);
//
//	unsigned int bitSrc[bitArraySize];
//	unsigned int bitDst[bitArraySize];
//	unsigned int table[newDimension];
//
//	for(unsigned int i = 0; i < bitArraySize; ++i)
//	{
//		if(oldDimension >= i*32 && oldDimension < (i+1)*32)
//			bitSrc[i] = 0 | ((1 << oldDimension%32)-1);
//		else if(oldDimension >= i*32 && oldDimension >= (i+1)*32)
//			bitSrc[i] = ~(bitSrc[i] & 0);
//		else
//			bitSrc[i] = 0;
//
//		bitDst[i] = ~(bitDst[i] & 0);
//	}
//
//	if(newDimension%32 != 0)
//	{
//		bitDst[bitArraySize-1] ^= ~((1 << newDimension % 32)-1);
//	}
//
//
//
//	dbm.resize(bitSrc, bitDst, bitArraySize, table);
//
//	for(int i = 0; i < nAdditionalTokens; ++i)
//	{
//		dbm(oldDimension+i) = 0; // reset new clocks to zero
//	}
//
//	std::cout << dbm << std::endl;
//
//	std::cout << "\nIndirection Table:\n";
//	for(int i = 0; i < oldDimension; ++i)
//		std::cout << i << ": " << table[i] << "\n";
//
////	std::vector<int> tokenMap;
////
////	tokenMap.push_back(-2);
////	tokenMap.push_back(2);
////	tokenMap.push_back(4);
////	tokenMap.push_back(3);
////	tokenMap.push_back(5);
////	tokenMap.push_back(1);
////
////	// recreate token map
////
////	std::vector<int> newTokenMap;
////
////	for(int i = 0; i < oldDimension; ++i)
////	{
////		if(table[i] != std::numeric_limits<unsigned int>().max())
////			newTokenMap.push_back(tokenMap[i]);
////	}
////
////	std::cout << "TokenMap:\n";
////	for(int i = 0; i < oldDimension; ++i)
////		std::cout << i << ": " << tokenMap[i] << "\n";
////
////	std::cout << "\nNew Token Map:\n";
////		for(int i = 0; i < newDimension; ++i)
////			std::cout << i << ": " << newTokenMap[i] << "\n";
//
//
//
//}

int main(int argc, char* argv[]) {

	VerificationOptions options = VerificationOptions::ParseVerificationOptions(argc, argv);

	TAPNXmlParser modelParser;
	boost::shared_ptr<TAPN::TimedArcPetriNet> tapn = modelParser.Parse(options.GetInputFile());
	tapn->Initialize();
	boost::shared_ptr<SymMarking> initialMarking = modelParser.ParseMarking(options.GetInputFile(), *tapn);
	initialMarking->MakeKBound(options.GetKBound());

	TAPNQueryParser queryParser(*tapn);
	queryParser.parse(options.QueryFile());
	AST::Query* query = queryParser.GetAST();

	SearchStrategy* strategy = NULL;

	strategy = new DFS(*tapn, *initialMarking, query, options);

	bool result = strategy->Execute();
	delete strategy;

	std::cout << "Query is " << (result ? "satisfied" : "NOT satisfied") << "." << std::endl;

	return 0;
}
