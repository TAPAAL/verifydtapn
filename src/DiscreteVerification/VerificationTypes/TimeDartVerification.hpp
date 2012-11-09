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
					&& (stack.size() > 2 && stack.top()->first->equals(*m->first))){
				root->append_node(doc.allocate_node(node_element, "loop"));
				foundLoop = true;
			}
			old = stack.top();
			stack.pop();
		}

		//Trace ended, goto * or deadlock
		if(query == AST::EG || query == AST::AF){
			if(!foundLoop && !delayedForever) {
				//TODO Deadlock conditiopn
				if(false){
					root->append_node(doc.allocate_node(node_element, "deadlock"));
				}else{
					// By default delay forever
					xml_node<>* node = doc.allocate_node(node_element, "delay", doc.allocate_string("forever"));
					for(PlaceList::const_iterator iter = m->first->places.begin(); iter != m->first->places.end(); iter++){
						if(iter->place->GetInvariant().GetBound() != std::numeric_limits<int>::max()){
							//Invariant, deadlock instead of delay forever
							node = doc.allocate_node(node_element, "deadlock");
							break;
						}
					}
					root->append_node(node);
				}
			}
		}

		std::cerr << doc;
	}

	xml_node<>* CreateTransitionNode(NonStrictMarking* old, NonStrictMarking* current, rapidxml::xml_document<>& doc){
		xml_node<>* transitionNode = doc.allocate_node(node_element, "transition");
		xml_attribute<>* id = doc.allocate_attribute("id", current->generatedBy->GetId().c_str());
		transitionNode->append_attribute(id);

		for(TAPN::TimedInputArc::WeakPtrVector::const_iterator arc_iter = current->generatedBy->GetPreset().begin(); arc_iter != current->generatedBy->GetPreset().end(); arc_iter++){
			createTransitionSubNodes(old, current, doc, transitionNode, arc_iter->lock()->InputPlace(), arc_iter->lock()->Interval(), arc_iter->lock()->GetWeight());
		}

		for(TAPN::TransportArc::WeakPtrVector::const_iterator arc_iter = current->generatedBy->GetTransportArcs().begin(); arc_iter != current->generatedBy->GetTransportArcs().end(); arc_iter++){
			createTransitionSubNodes(old, current, doc, transitionNode, arc_iter->lock()->Source(), arc_iter->lock()->Interval(), arc_iter->lock()->GetWeight());
		}

		return transitionNode;
	}
};

}
}

#endif
