#ifndef VERIFICATION_HPP_
#define VERIFICATION_HPP_

#include "DiscreteVerification/DataStructures/NonStrictMarking.hpp"
#include "../DeadlockVisitor.hpp"

#include "Core/TAPN/TimedPlace.hpp"

#include <stack>
#include <iostream>

#include <rapidxml.hpp>
/* Adding declarations to make it compatible with gcc 4.7 and greater */
namespace rapidxml { namespace internal {
    template<class OutIt, class Ch>
    inline OutIt print_children(OutIt out, const xml_node <Ch> *node, int flags, int indent);

    template<class OutIt, class Ch>
    inline OutIt print_attributes(OutIt out, const xml_node <Ch> *node, int flags);

    template<class OutIt, class Ch>
    inline OutIt print_data_node(OutIt out, const xml_node <Ch> *node, int flags, int indent);

    template<class OutIt, class Ch>
    inline OutIt print_cdata_node(OutIt out, const xml_node <Ch> *node, int flags, int indent);

    template<class OutIt, class Ch>
    inline OutIt print_element_node(OutIt out, const xml_node <Ch> *node, int flags, int indent);

    template<class OutIt, class Ch>
    inline OutIt print_declaration_node(OutIt out, const xml_node <Ch> *node, int flags, int indent);

    template<class OutIt, class Ch>
    inline OutIt print_comment_node(OutIt out, const xml_node <Ch> *node, int flags, int indent);

    template<class OutIt, class Ch>
    inline OutIt print_doctype_node(OutIt out, const xml_node <Ch> *node, int flags, int indent);

    template<class OutIt, class Ch>
    inline OutIt print_pi_node(OutIt out, const xml_node <Ch> *node, int flags, int indent);
} }

#include <rapidxml_print.hpp>
#include <utility>
#include <utility>

namespace VerifyTAPN { namespace DiscreteVerification {
    template<typename T>
    class Verification {
    public:

        Verification(TAPN::TimedArcPetriNet &tapn, T &initialMarking, AST::Query *query,
                     VerificationOptions options);

        virtual ~Verification() = default;

        virtual bool run() = 0;

        virtual void printStats() = 0;

        virtual void printTransitionStatistics() const = 0;

        virtual void printPlaceStatistics();

        virtual unsigned int maxUsedTokens() = 0;

        virtual bool handleSuccessor(T *marking) = 0;

        virtual void getTrace() {
            std::cout << "Error generating trace" << std::endl;
        }

        void removeLastIfDelay(rapidxml::xml_node<> &);

        void printHumanTrace(T *m, std::stack<T *> &stack, AST::Quantifier query);

        void printXMLTrace(T *m, std::stack<T *> &stack, AST::Query *query, TAPN::TimedArcPetriNet &tapn);

        rapidxml::xml_node<> *createTransitionNode(T *old, T *current, rapidxml::xml_document<> &doc);

        void createTransitionSubNodes(T *old, T *current, rapidxml::xml_document<> &doc,
                                      rapidxml::xml_node<> *transitionNode, const TAPN::TimedPlace &place,
                                      const TAPN::TimeInterval &interval, int weight);

        rapidxml::xml_node<> *
        createTokenNode(rapidxml::xml_document<> &doc, const TAPN::TimedPlace &place, const Token &token);

        void generateTraceStack(T *m, std::stack<T *> *result, std::stack<T *> *liveness = nullptr);

        std::stack<T *> trace{};
    protected:
        TAPN::TimedArcPetriNet &tapn;
        T &initialMarking;
        AST::Query *query;
        VerificationOptions options;
        std::vector<int> placeStats{};

    };

    template<typename T>
    Verification<T>::Verification(TAPN::TimedArcPetriNet &tapn, T &initialMarking, AST::Query *query,
                                  VerificationOptions options)
            : tapn(tapn), initialMarking(initialMarking), query(query), options(std::move(std::move(options))),
              placeStats(tapn.getNumberOfPlaces()) {

    }

    template<typename T>
    void Verification<T>::printPlaceStatistics() {
        fprintf(stdout, "PLACE-BOUND STATISTICS\n");
        for (size_t p = 0; p < placeStats.size(); ++p) {
            // report maximum bounds for each place (? means that the place had some deadlock tokens that were removed)
            int count = placeStats[p];
            const TAPN::TimedPlace place = tapn.getPlace(p);
            if (count == std::numeric_limits<int>::max()) {
                fprintf(stdout, "<%s;?> ", place.getName().c_str());
            } else {
                fprintf(stdout, "<%s;%i> ", place.getName().c_str(), count);
            }
        }
        fprintf(stdout, "\n\n");
    }

    template<typename T>
    void Verification<T>::printHumanTrace(T *m, std::stack<T *> &stack, AST::Quantifier query) {
        std::cout << "Trace: " << std::endl;
        bool isFirst = true;
        bool foundLoop = false;

        while (!stack.empty()) {
            if (isFirst) {
                isFirst = false;
            } else {
                if (stack.top()->getGeneratedBy()) {
                    std::cout << "\tTransistion: " << stack.top()->getGeneratedBy()->getName() << std::endl;
                } else {
                    int i = 1;
                    T *old = stack.top();
                    stack.pop();
                    while (!stack.empty() && stack.top()->getGeneratedBy() == nullptr) {
                        old = stack.top();
                        stack.pop();
                        i++;
                    }

                    if ((!foundLoop) && stack.empty() && old->getNumberOfChildren() > 0) {
                        std::cout << "\tDelay: Forever" << std::endl;
                        return;
                    }

                    std::cout << "\tDelay: " << i << std::endl;
                    stack.push(old);
                }
            }

            if ((query == AST::EG || query == AST::AF)
                && (stack.size() > 1 && stack.top()->equals(*m))
                && (m->getGeneratedBy() || stack.top()->getParent())) {
                std::cout << "\t* ";
                foundLoop = true;
            } else {
                std::cout << "\t";
            }

            //Print marking
            std::cout << "Marking: ";
            for (auto& token_list : stack.top()->getPlaceList()) {
                for (auto& token : token_list.tokens) {
                    for (int i = 0; i < token.getCount(); i++) {
                        std::cout << "(" << token_list.place->getName() << "," << token.getAge() << ") ";
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
                if (m->getNumberOfChildren() > 0) {
                    std::cout << "\tDeadlock" << std::endl;
                } else {
                    for (auto iter = m->getPlaceList().begin();
                         iter != m->getPlaceList().end(); iter++) {
                        if (iter->place->getInvariant().getBound() != std::numeric_limits<int>::max()) {
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
    void Verification<T>::removeLastIfDelay(rapidxml::xml_node<> &root) {
        using namespace rapidxml;
        if (root.first_node()) { // if there is a node in the trace
            xml_node<> *node = root.last_node();
            if (node) {
                char *name = node->name();
                if (strcmp(name, "delay") == 0) {
                    root.remove_last_node();
                }
            }
        }
    }

    template<typename T>
    void
    Verification<T>::printXMLTrace(T *m, std::stack<T *> &stack, AST::Query *query, TAPN::TimedArcPetriNet &tapn) {
        using namespace rapidxml;
        std::cerr << "Trace: " << std::endl;
        bool isFirst = true;
        bool foundLoop = false;
        bool delayedForever = false;
        T *old = nullptr;

        xml_document<> doc;
        xml_node<> *root = doc.allocate_node(node_element, "trace");
        doc.append_node(root);

        while (!stack.empty()) {
            if (isFirst) {
                isFirst = false;
            } else {
                if (stack.top()->getGeneratedBy()) {
                    root->append_node(createTransitionNode(old, stack.top(), doc));
                } else {
                    int i = 1;
                    old = stack.top();
                    stack.pop();
                    bool delayloop = false;
                    while (!stack.empty() && stack.top()->getGeneratedBy() == nullptr) {
                        // check if this marking is the start of a loop
                        if (!foundLoop && (query->getQuantifier() == AST::EG || query->getQuantifier() == AST::AF)
                            && (stack.size() > 2 && old->equals(*m))) {

                            foundLoop = true;
                            delayloop = true;
                            auto str = std::to_string(i * tapn.getGCD());
                            xml_node<> *node = doc.allocate_node(node_element, "delay", doc.allocate_string(
                                    str.c_str()));
                            root->append_node(node);
                            root->append_node(doc.allocate_node(node_element, "loop"));

                        }
                        if (delayloop)
                            break;
                        old = stack.top();
                        stack.pop();
                        i++;

                    }
                    if (delayloop)
                        continue;
                    if ((!foundLoop) && stack.empty() && old->getNumberOfChildren() > 0) {
                        // remove delay before delay forever
                        removeLastIfDelay(*root);
                        xml_node<> *node = doc.allocate_node(node_element, "delay", doc.allocate_string("forever"));
                        root->append_node(node);
                        delayedForever = true;
                        break;
                    }
                    // if we are looking at the last marking and we have some delay
                    // then in case of deadlock we want to find the earliest point
                    // with a transition enabled, and add one (such that we have a
                    // deadlock with no end-delay).
                    if ((!foundLoop) && stack.empty()) {
                        // make sure query contains deadlock
                        DeadlockVisitor deadlockVisitor = DeadlockVisitor();
                        AST::BoolResult queryContainsDeadlock;
                        deadlockVisitor.visit(*query, queryContainsDeadlock);

                        if (queryContainsDeadlock.value) {
                            // find the marking from which a delay gave a deadlock
                            NonStrictMarkingBase *base = old;
                            while (base->getGeneratedBy() == nullptr && base->getParent() != nullptr) {
                                base = base->getParent();
                            }

                            // decrement the last marking until earliest delay with a transition enabled.
                            while (i > 0 && base->canDeadlock(tapn, i, true)) {
                                i--;
                            }
                            // add one to make a deadlock again if we have not reached zero delay and zero delay is a deadlock
                            if (!base->canDeadlock(tapn, i, true)) {
                                i++;
                            }
                        }
                    }
                    if (i > 0) {
                        auto str = std::to_string(i * tapn.getGCD());
                        xml_node<> *node = doc.allocate_node(node_element, "delay", doc.allocate_string(
                                str.c_str()));
                        root->append_node(node);
                    }
                    stack.push(old);
                }
            }

            if ((query->getQuantifier() == AST::EG || query->getQuantifier() == AST::AF)
                && (stack.size() > 1 && stack.top()->equals(*m))) {
                T *temp = m;
                foundLoop = false;
                T *top = stack.top();

                do {
                    if (temp == top)
                        break;
                    if (temp->getGeneratedBy()) {
                        foundLoop = true;
                        break;
                    }
                    temp = (T *) temp->getParent();
                } while (temp && temp->getParent());
                if (foundLoop) {
                    root->append_node(doc.allocate_node(node_element, "loop"));
                }

            }
            old = stack.top();
            stack.pop();
        }

        //Trace ended, goto * or deadlock
        if (query->getQuantifier() == AST::EG || query->getQuantifier() == AST::AF) {
            if (!foundLoop && !delayedForever) {
                xml_node<> *node;
                if (m->canDeadlock(tapn, 0, false)) {
                    // check if deadlock
                    node = doc.allocate_node(node_element, "deadlock");
                } else {
                    // if not it is delay forever

                    // remove delay before delay forever
                    removeLastIfDelay(*root);
                    node = doc.allocate_node(node_element, "delay", doc.allocate_string("forever"));

                }
                root->append_node(node);
            }
        }
        std::cerr << doc;
    }

    template<typename T>
    rapidxml::xml_node<> *Verification<T>::createTransitionNode(T *old, T *current, rapidxml::xml_document<> &doc) {
        using namespace rapidxml;
        xml_node<> *transitionNode = doc.allocate_node(node_element, "transition");
        xml_attribute<> *id = doc.allocate_attribute("id", current->getGeneratedBy()->getId().c_str());
        transitionNode->append_attribute(id);

        for (auto* arc : current->getGeneratedBy()->getPreset()) {
            createTransitionSubNodes(old, current, doc, transitionNode, arc->getInputPlace(),
                                     arc->getInterval(), arc->getWeight());
        }

        for (auto* arc : current->getGeneratedBy()->getTransportArcs()) {
            createTransitionSubNodes(old, current, doc, transitionNode, arc->getSource(),
                                     arc->getInterval(), arc->getWeight());
        }

        return transitionNode;
    }

    template<typename T>
    void Verification<T>::createTransitionSubNodes(T *old, T *current, rapidxml::xml_document<> &doc,
                                                   rapidxml::xml_node<> *transitionNode,
                                                   const TAPN::TimedPlace &place,
                                                   const TAPN::TimeInterval &interval, const int weight) {
        TokenList current_tokens = current->getTokenList(place.getIndex());
        TokenList old_tokens = old->getTokenList(place.getIndex());
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

        for (TokenList::const_iterator iter = n_iter; iter != current_tokens.end(); iter++) {
            for (int i = 0; i < iter->getCount(); i++) {
                transitionNode->append_node(createTokenNode(doc, place, *iter));
                tokensFound++;
            }
        }

        for (auto& token : old_tokens) {
            if(tokensFound >= weight) break;
            if (token.getAge() >= interval.getLowerBound()) {
                for (int i = 0; i < token.getCount() && tokensFound < weight; i++) {
                    transitionNode->append_node(createTokenNode(doc, place, token));
                    tokensFound++;
                }
            }
        }
    }

    template<typename T>
    rapidxml::xml_node<> *
    Verification<T>::createTokenNode(rapidxml::xml_document<> &doc, const TAPN::TimedPlace &place,
                                     const Token &token) {
        using namespace rapidxml;
        xml_node<> *tokenNode = doc.allocate_node(node_element, "token");
        xml_attribute<> *placeAttribute = doc.allocate_attribute("place",
                                                                 doc.allocate_string(place.getName().c_str()));
        tokenNode->append_attribute(placeAttribute);
        auto str = std::to_string(token.getAge() * tapn.getGCD());
        xml_attribute<> *ageAttribute = doc.allocate_attribute("age", doc.allocate_string(
                str.c_str()));
        tokenNode->append_attribute(ageAttribute);
        if (place.getMaxConstant() < token.getAge()) {
            xml_attribute<> *gtAttribute = doc.allocate_attribute("greaterThanOrEqual",
                                                                  doc.allocate_string("true"));
            tokenNode->append_attribute(gtAttribute);
        } else {
            xml_attribute<> *gtAttribute = doc.allocate_attribute("greaterThanOrEqual",
                                                                  doc.allocate_string("false"));
            tokenNode->append_attribute(gtAttribute);
        }

        return tokenNode;
    }

    template<typename T>
    void Verification<T>::generateTraceStack(T *m, std::stack<T *> *result, std::stack<T *> *liveness) {
        if (liveness == nullptr) {
            T *next = m;
            do {
                result->push((T *) next);
            } while ((next = ((T *) next->getParent())) != nullptr);
        } else {
            do {
                result->push(liveness->top());
                liveness->pop();
                if (!liveness->empty()) {
                    result->top()->setParent(liveness->top());
                }
            } while (!(liveness->empty()));
        }
    }
} }

#endif
