#ifndef VERIFICATION_HPP_
#define VERIFICATION_HPP_

#include "../../../lib/rapidxml-1.13/rapidxml.hpp"
#include "../../../lib/rapidxml-1.13/rapidxml_print.hpp"
#include "../DataStructures/NonStrictMarking.hpp"
#include <stack>
#include "../../Core/TAPNParser/util.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

class Verification {
public:
virtual bool Verify() = 0;
virtual ~Verification(){};
virtual void printStats() = 0;
virtual void PrintTransitionStatistics() const = 0;
virtual unsigned int MaxUsedTokens() = 0;
virtual NonStrictMarking* GetLastMarking() = 0;
virtual void GetTrace(bool XMLTrace){ std::cout << "Error generating trace" << std::endl; }

void PrintHumanTrace(NonStrictMarking* m, std::stack<NonStrictMarking*>& stack, AST::Quantifier query);
void PrintXMLTrace(NonStrictMarking* m, std::stack<NonStrictMarking*>& stack, AST::Quantifier query);
rapidxml::xml_node<>* CreateTransitionNode(NonStrictMarking* old, NonStrictMarking* current, rapidxml::xml_document<>& doc);
void createTransitionSubNodes(NonStrictMarking* old, NonStrictMarking* current, rapidxml::xml_document<>& doc, rapidxml::xml_node<>* transitionNode, const TAPN::TimedPlace& place, const TAPN::TimeInterval& interval, const int weight);
rapidxml::xml_node<>* createTokenNode(rapidxml::xml_document<>& doc, const TAPN::TimedPlace& place, const Token& token);
void GenerateTraceStack(NonStrictMarking* m, std::stack<NonStrictMarking*>* result ,std::stack<NonStrictMarking*>* liveness = NULL);
};

}
}

#endif
