#ifndef TIMEDARTVERIFICATION_HPP_
#define TIMEDARTVERIFICATION_HPP_

#include "Verification.hpp"
#include "../DataStructures/TimeDart.hpp"
#include <stack>

namespace VerifyTAPN {
namespace DiscreteVerification {

using namespace rapidxml;

typedef pair<NonStrictMarking*, int> TraceList;

class TimeDartVerification : public Verification {
public:

	void PrintXMLTrace(TraceList* m, std::stack<TraceList*>& stack, Quantifier query) {
		std::cerr << "Trace: " << std::endl;
		TraceList* old = stack.top();
		stack.pop();
		bool foundLoop = false;
		bool delayedForever = false;

		xml_document<> doc;
		xml_node<>* root = doc.allocate_node(node_element, "trace");
		doc.append_node(root);

		while(!stack.empty()){
			if(old->first != NULL){
				root->append_node(CreateTransitionNode(old->first, stack.top()->first, doc));
			}

			if(stack.size() > 1 && stack.top()->second > 0){
				xml_node<>* node = doc.allocate_node(node_element, "delay", doc.allocate_string(ToString(stack.top()->second).c_str()));
				root->append_node(node);
				stack.top()->first->incrementAge(stack.top()->second);
			}


			if((query == AST::EG || query == AST::AF)
					&& (stack.size() > 1 && stack.top()->first->equals(*m->first))){
				root->append_node(doc.allocate_node(node_element, "loop"));
				foundLoop = true;
			}
			old = stack.top();
			stack.pop();
		}

		//Trace ended, goto * or deadlock
		if(query == AST::EG || query == AST::AF){
			if(!foundLoop && !delayedForever) {
				// By default delay forever
				xml_node<>* node = doc.allocate_node(node_element, "delay", doc.allocate_string("forever"));
				for(PlaceList::const_iterator iter = m->first->places.begin(); iter != m->first->places.end(); iter++){
					if(iter->place->GetInvariant().GetBound() != std::numeric_limits<int>::max()){
						//Invariant, deadlock instead of delay forever
						if(m->second > 0){
							xml_node<>* node = doc.allocate_node(node_element, "delay", doc.allocate_string(ToString(m->second).c_str()));
							root->append_node(node);
						}
						node = doc.allocate_node(node_element, "deadlock");
						break;
					}
				}
				root->append_node(node);
			}
		}

		std::cerr << doc;
	}
};

}
}

#endif
