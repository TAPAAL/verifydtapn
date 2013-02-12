#ifndef VERIFICATION_HPP_
#define VERIFICATION_HPP_

#include "../../../lib/rapidxml-1.13/rapidxml.hpp"
#include "../../../lib/rapidxml-1.13/rapidxml_print.hpp"
#include "../DataStructures/NonStrictMarking.hpp"
#include <stack>
#include "../../Core/TAPNParser/util.hpp"

namespace VerifyTAPN {
    namespace DiscreteVerification {
        template<typename T>
        class Verification {
        public:
            virtual bool Verify() = 0;

            virtual ~Verification() {
            };
            virtual void printStats() = 0;
            virtual void PrintTransitionStatistics() const = 0;
            virtual unsigned int MaxUsedTokens() = 0;

            virtual void GetTrace() {
                std::cout << "Error generating trace" << std::endl;
            }

            void PrintHumanTrace(T* m, std::stack<T*>& stack, AST::Quantifier query);
            void PrintXMLTrace(T* m, std::stack<T*>& stack, AST::Quantifier query);
            rapidxml::xml_node<>* CreateTransitionNode(T* old, T* current, rapidxml::xml_document<>& doc);
            void createTransitionSubNodes(T* old, T* current, rapidxml::xml_document<>& doc, rapidxml::xml_node<>* transitionNode, const TAPN::TimedPlace& place, const TAPN::TimeInterval& interval, const int weight);
            rapidxml::xml_node<>* createTokenNode(rapidxml::xml_document<>& doc, const TAPN::TimedPlace& place, const Token& token);
            void GenerateTraceStack(T* m, std::stack<T*>* result, std::stack<T*>* liveness = NULL);
            stack< T* > trace;
        };

        template<typename T>
        void Verification<T>::PrintHumanTrace(T* m, std::stack<T*>& stack, AST::Quantifier query) {
            std::cout << "Trace: " << std::endl;
            bool isFirst = true;
            bool foundLoop = false;

            while (!stack.empty()) {
                if (isFirst) {
                    isFirst = false;
                } else {
                    if (stack.top()->GetGeneratedBy()) {
                        std::cout << "\tTransistion: " << stack.top()->GetGeneratedBy()->GetName() << std::endl;
                    } else {
                        int i = 1;
                        T* old = stack.top();
                        stack.pop();
                        while (!stack.empty() && stack.top()->GetGeneratedBy() == NULL) {
                            old = stack.top();
                            stack.pop();
                            i++;
                        }

                        if (stack.empty() && old->children > 0) {
                            std::cout << "\tDelay: Forever" << std::endl;
                            return;
                        }

                        std::cout << "\tDelay: " << i << std::endl;
                        stack.push(old);
                    }
                }

                if ((query == AST::EG || query == AST::AF)
                        && (stack.size() > 1 && stack.top()->equals(*m))
                        && (m->GetGeneratedBy() || stack.top()->parent)) {
                    std::cout << "\t* ";
                    foundLoop = true;
                } else {
                    std::cout << "\t";
                }

                //Print marking
                std::cout << "Marking: ";
                for (PlaceList::const_iterator iter = stack.top()->places.begin(); iter != stack.top()->places.end(); iter++) {
                    for (TokenList::const_iterator titer = iter->tokens.begin(); titer != iter->tokens.end(); titer++) {
                        for (int i = 0; i < titer->getCount(); i++) {
                            std::cout << "(" << iter->place->GetName() << "," << titer->getAge() << ") ";
                        }
                    }
                }

                std::cout << std::endl;
                stack.pop();
            }

            //Trace ended, goto * or deadlock
            if (query == AST::EG || query == AST::AF) {
                if (foundLoop) {
                    std::cout << "\tgoto *" << std::endl;
                } else {
                    if (m->children > 0) {
                        std::cout << "\tDeadlock" << std::endl;
                    } else {
                        for (PlaceList::const_iterator iter = m->places.begin(); iter != m->places.end(); iter++) {
                            if (iter->place->GetInvariant().GetBound() != std::numeric_limits<int>::max()) {
                                //Invariant, deadlock
                                std::cout << "\tDeadlock" << std::endl;
                                return;
                            }
                        }
                        std::cout << "\tDelay: Forever" << std::endl;
                    }
                }

            }
        }

        template<typename T>
        void Verification<T>::PrintXMLTrace(T* m, std::stack<T*>& stack, AST::Quantifier query) {
            using namespace rapidxml;
            std::cerr << "Trace: " << std::endl;
            bool isFirst = true;
            bool foundLoop = false;
            bool delayedForever = false;
            T* old;

            xml_document<> doc;
            xml_node<>* root = doc.allocate_node(node_element, "trace");
            doc.append_node(root);

            while (!stack.empty()) {
                if (isFirst) {
                    isFirst = false;
                } else {
                    if (stack.top()->GetGeneratedBy()) {
                        root->append_node(CreateTransitionNode(old, stack.top(), doc));
                    } else {
                        int i = 1;
                        old = stack.top();
                        stack.pop();
                        while (!stack.empty() && stack.top()->GetGeneratedBy() == NULL) {
                            old = stack.top();
                            stack.pop();
                            i++;
                        }

                        if (stack.empty() && old->children > 0) {
                            xml_node<>* node = doc.allocate_node(node_element, "delay", doc.allocate_string("forever"));
                            root->append_node(node);
                            delayedForever = true;
                            break;
                        }
                        xml_node<>* node = doc.allocate_node(node_element, "delay", doc.allocate_string(ToString(i).c_str()));
                        root->append_node(node);
                        stack.push(old);
                    }
                }
                
                if ((query == AST::EG || query == AST::AF)
                        && (stack.size() > 1 && stack.top()->equals(*m))
                        && (m->GetGeneratedBy() || stack.top()->parent)) {
                    root->append_node(doc.allocate_node(node_element, "loop"));
                    foundLoop = true;
                }
                old = stack.top();
                stack.pop();
            }

            //Trace ended, goto * or deadlock
            if (query == AST::EG || query == AST::AF) {
                if (!foundLoop && !delayedForever) {
                    if (m->children > 0) {
                        root->append_node(doc.allocate_node(node_element, "deadlock"));
                    } else {
                        // By default delay forever
                        xml_node<>* node = doc.allocate_node(node_element, "delay", doc.allocate_string("forever"));
                        for (PlaceList::const_iterator iter = m->places.begin(); iter != m->places.end(); iter++) {
                            if (iter->place->GetInvariant().GetBound() != std::numeric_limits<int>::max()) {
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
        
        template<typename T>
        rapidxml::xml_node<>* Verification<T>::CreateTransitionNode(T* old, T* current, rapidxml::xml_document<>& doc) {
            using namespace rapidxml;
            xml_node<>* transitionNode = doc.allocate_node(node_element, "transition");
            xml_attribute<>* id = doc.allocate_attribute("id", current->generatedBy->GetId().c_str());
            transitionNode->append_attribute(id);

            for (TAPN::TimedInputArc::WeakPtrVector::const_iterator arc_iter = current->generatedBy->GetPreset().begin(); arc_iter != current->generatedBy->GetPreset().end(); arc_iter++) {
                createTransitionSubNodes(old, current, doc, transitionNode, arc_iter->lock()->InputPlace(), arc_iter->lock()->Interval(), arc_iter->lock()->GetWeight());
            }

            for (TAPN::TransportArc::WeakPtrVector::const_iterator arc_iter = current->generatedBy->GetTransportArcs().begin(); arc_iter != current->generatedBy->GetTransportArcs().end(); arc_iter++) {
                createTransitionSubNodes(old, current, doc, transitionNode, arc_iter->lock()->Source(), arc_iter->lock()->Interval(), arc_iter->lock()->GetWeight());
            }

            return transitionNode;
        }
        template<typename T>
        void Verification<T>::createTransitionSubNodes(T* old, T* current, rapidxml::xml_document<>& doc, rapidxml::xml_node<>* transitionNode, const TAPN::TimedPlace& place, const TAPN::TimeInterval& interval, const int weight) {
            TokenList current_tokens = current->GetTokenList(place.GetIndex());
            TokenList old_tokens = old->GetTokenList(place.GetIndex());
            int tokensFound = 0;

            TokenList::const_iterator n_iter = current_tokens.begin();
            TokenList::const_iterator o_iter = old_tokens.begin();
            while (n_iter != current_tokens.end() && o_iter != old_tokens.end()) {
                if (n_iter->getAge() == o_iter->getAge()) {
                    for (int i = 0; i < o_iter->getCount() - n_iter->getCount(); i++) {
                        transitionNode->append_node(createTokenNode(doc, place, *n_iter));
                        tokensFound++;
                    }
                    n_iter++;
                    o_iter++;
                } else {
                    if (n_iter->getAge() > o_iter->getAge()) {
                        transitionNode->append_node(createTokenNode(doc, place, *o_iter));
                        tokensFound++;
                        o_iter++;
                    } else {
                        n_iter++;
                    }
                }
            }

            for (TokenList::const_iterator iter = o_iter; iter != old_tokens.end(); iter++) {
                for (int i = 0; i < iter->getCount(); i++) {
                    transitionNode->append_node(createTokenNode(doc, place, *iter));
                    tokensFound++;
                }
            }

            for (TokenList::const_iterator iter = old_tokens.begin(); iter != old_tokens.end() && tokensFound < weight; iter++) {
                if (iter->getAge() >= interval.GetLowerBound()) {
                    for (int i = 0; i < iter->getCount() && tokensFound < weight; i++) {
                        transitionNode->append_node(createTokenNode(doc, place, *iter));
                        tokensFound++;
                    }
                }
            }
        }

        template<typename T>
        rapidxml::xml_node<>* Verification<T>::createTokenNode(rapidxml::xml_document<>& doc, const TAPN::TimedPlace& place, const Token& token) {
            using namespace rapidxml;
            xml_node<>* tokenNode = doc.allocate_node(node_element, "token");
            xml_attribute<>* placeAttribute = doc.allocate_attribute("place", doc.allocate_string(place.GetName().c_str()));
            tokenNode->append_attribute(placeAttribute);
            xml_attribute<>* ageAttribute = doc.allocate_attribute("age", doc.allocate_string(ToString(token.getAge()).c_str()));
            tokenNode->append_attribute(ageAttribute);
            if (place.GetMaxConstant() < token.getAge()) {
                xml_attribute<>* gtAttribute = doc.allocate_attribute("greaterThanOrEqual", doc.allocate_string("true"));
                tokenNode->append_attribute(gtAttribute);
            } else {
                xml_attribute<>* gtAttribute = doc.allocate_attribute("greaterThanOrEqual", doc.allocate_string("false"));
                tokenNode->append_attribute(gtAttribute);
            }

            return tokenNode;
        }

        template<typename T>
        void Verification<T>::GenerateTraceStack(T* m, std::stack<T*>* result, std::stack<T*>* liveness) {
            if (liveness == NULL) {
                T* next = m;
                do {
                    
                    result->push((T*)next);
                } while ((next = ((T*)next->parent)) != NULL);
            } else {
                do {
                    result->push(liveness->top());
                    liveness->pop();
                } while (!(liveness->empty()));
            }
        }
    }
}

#endif
