/*
 * DiscreteVerification.hpp
 *
 *  Created on: 23 Feb 2012
 *      Author: jakob
 */

#ifndef DISCRETEVERIFICATION_HPP_
#define DISCRETEVERIFICATION_HPP_

#include <iostream>
#include "boost/smart_ptr.hpp"
#include "../Core/TAPN/TAPN.hpp"
#include "../Core/QueryParser/AST.hpp"
#include "../Core/VerificationOptions.hpp"
#include "DataStructures/NonStrictMarking.hpp"
#include "../../lib/rapidxml-1.13/rapidxml.hpp"
#include "../../lib/rapidxml-1.13/rapidxml_print.hpp"
#include "SearchStrategies/NonStrictDFS.hpp"
#include "SearchStrategies/NonStrictBFS.hpp"
#include "SearchStrategies/NonStrictHeuristic.hpp"
#include "SearchStrategies/NonStrictRandom.hpp"
#include "NonStrictSearch.hpp"
#include "SearchStrategies/NonStrictDFSHeuristic.hpp"
#include "SearchStrategies/NonStrictDFSRandom.hpp"
#include "../Core/TAPNParser/util.hpp"
#include <stack>


namespace VerifyTAPN {

namespace DiscreteVerification {

class DiscreteVerification {
public:
	DiscreteVerification();
	virtual ~DiscreteVerification();
	static int run(boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn, std::vector<int> initialPlacement, AST::Query* query, VerificationOptions options);
private:
	static void PrintHumanTrace(bool result, NonStrictMarking* m, std::stack<NonStrictMarking*>& stack, AST::Quantifier query);
	static void PrintXMLTrace(bool result, NonStrictMarking* m, std::stack<NonStrictMarking*>& stack, AST::Quantifier query);
	static rapidxml::xml_node<>* CreateTransitionNode(NonStrictMarking* old, NonStrictMarking* current, rapidxml::xml_document<>& doc);
	static rapidxml::xml_node<>* createTokenNode(rapidxml::xml_document<>& doc, const TAPN::TimedPlace& place, const Token& token);
	static void createTransitionSubNodes(NonStrictMarking* old, NonStrictMarking* current, rapidxml::xml_document<>& doc, rapidxml::xml_node<>* transitionNode, const TAPN::TimedPlace& place, const TAPN::TimeInterval& interval, const int weight);
	static void GenerateTraceStack(NonStrictMarking* m, std::stack<NonStrictMarking*>* stack, std::stack<NonStrictMarking*>* liveness = NULL);
};

}

}

#endif /* DISCRETEVERIFICATION_HPP_ */
