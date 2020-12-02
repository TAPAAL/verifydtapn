/* 
 * File:   SafetySynthesis.cpp
 * Author: Peter G. Jensen
 * 
 * Created on 14 September 2015, 15:59
 */
#include "DiscreteVerification/VerificationTypes/SafetySynthesis.h"
#include "DiscreteVerification/DataStructures/SimpleMarkingStore.h"
#include "DiscreteVerification/DataStructures/PTrieMarkingStore.h"
#include "DiscreteVerification/Generators/ReducingGameGenerator.h"

#include <cassert>
#include <set>


namespace VerifyTAPN { namespace DiscreteVerification {

    SafetySynthesis::SafetySynthesis(TAPN::TimedArcPetriNet &tapn,
                                     NonStrictMarking &initialMarking,
                                     AST::Query *query,
                                     VerificationOptions &options)
            : tapn(tapn), initial_marking(initialMarking),
              query(query), options(options),
              placeStats(tapn.getNumberOfPlaces()),
              generator(nullptr), discovered(0), explored(0),
              largest(0) {
        if (options.getMemoryOptimization() == VerificationOptions::PTRIE)
            store = new PTrieMarkingStore<SafetyMeta>(tapn, options.getKBound());
        else
            store = new SimpleMarkingStore<SafetyMeta>();

        switch (options.getSearchType()) {
            case VerificationOptions::DEPTHFIRST:
                waiting = new dfs_queue<store_t::Pointer *>();
                break;
            case VerificationOptions::RANDOM:
                waiting = new random_queue<store_t::Pointer *>();
                break;
            case VerificationOptions::BREADTHFIRST:
            default:
                waiting = new bfs_queue<store_t::Pointer *>();
                break;
        }
        if(options.getPartialOrderReduction())
        {
            if(query->getQuantifier() == CG)
            {
                std::cerr << "Partial order reduction currently only supported for control: AF, not control: AG" << std::endl;
                assert(false);
                std::exit(-1);
            }
            generator = std::make_unique<ReducingGameGenerator>(tapn, query);
        }
        else
            generator = std::make_unique<GameGenerator>(tapn, query);
    }

    bool SafetySynthesis::satisfies_query(NonStrictMarkingBase *m) {
        m->cut(placeStats);
        if (m->size() > options.getKBound()) return false;
        QueryVisitor<NonStrictMarkingBase> checker(*m, tapn);
        BoolResult context;
        query->accept(checker, context);
        return context.value;
    }

    bool SafetySynthesis::run() {
        backstack_t back;
        largest = initial_marking.size();
        
        // if initial satisfies and AF (return true), ok OR initial violates and AG (return false)
        if (satisfies_query(&initial_marking) == (query->getQuantifier() == Quantifier::CF))
            return query->getQuantifier() == Quantifier::CF;

        store_t::result_t m_0_res = store->insert_and_dealloc(&initial_marking);

        SafetyMeta &meta = store->get_meta(m_0_res.second);
        meta = {UNKNOWN, false, 0, 0, depends_t()};
        meta.waiting = true;

        waiting->push(m_0_res.second);

        while (meta.state != LOOSING && meta.state != WINNING) {
            ++explored;
            store_t::Pointer *next;

            while (!back.empty()) {
                next = back.top();
                back.pop();
                dependers_to_waiting(store->get_meta(next), back);
            }

            if (waiting->empty()) break;

            next = waiting->pop();

            SafetyMeta &next_meta = store->get_meta(next);
//        std::cout   << "pop " << next << " State: " << &next_meta << " - > "
//                    << next_meta.state << std::endl;

            next_meta.waiting = false;
            if (next_meta.state == LOOSING ||
                next_meta.state == WINNING) {
                // these are allready handled in back stack
                continue;
            } else {
                assert(next_meta.state == UNKNOWN);
                next_meta.state = PROCESSED;
                //std::cerr << "PRE META " << meta.state << std::endl;
                NonStrictMarkingBase *marking = store->expand(next);
                // generate successors for environment
                generator->prepare(marking);
                successors(next, next_meta, *waiting, false, query);

                if (next_meta.state != LOOSING) {
                    // generate successors for controller
                    successors(next, next_meta, *waiting, true, query);
                }
                
                //std::cerr << "CHILDREN (" << next_meta.env_children << ", " << next_meta.ctrl_children << ")" << std::endl;
                //std::cerr << "NEW META " << next_meta.state << std::endl;

                if (next_meta.state == MAYBE_WINNING && next_meta.env_children == 0) {
                    next_meta.state = WINNING;
                }

                if (next_meta.state == LOOSING || next_meta.state == WINNING) {
                    assert(store->get_meta(next).state != PROCESSED);
                    //std::cerr << "PUSH " << &next_meta << std::endl;
                    back.push(next);
                }
                store->free(marking);
            }
            //std::cerr << "META " << meta.state << std::endl;
        }
        
        if(query->getQuantifier() == Quantifier::CG)
            return meta.state != LOOSING;
        else if(query->getQuantifier() == Quantifier::CF)
            return meta.state == WINNING;
        else
        {
            assert(false);
            return false;
        }
    }

    void SafetySynthesis::dependers_to_waiting(SafetyMeta &next_meta, backstack_t &back) {
        //std::cerr << "DEP TO WAITING " << &next_meta << " STATE " << next_meta.state << std::endl;
        for (const auto& ancestor : next_meta.dependers) {
            SafetyMeta &a_meta = store->get_meta(ancestor.second);
            //std::cerr << "\tDEP" << &a_meta << " STATE " << a_meta.state << std::endl;
            if (a_meta.state == LOOSING || a_meta.state == WINNING) continue;

            bool ctrl_child = ancestor.first;
            if (ctrl_child) {
                a_meta.ctrl_children -= 1;
                if (next_meta.state == WINNING && a_meta.state == MAYBE_LOSING)
                    a_meta.state = WINNING;                
                else if (next_meta.state == WINNING) {
                    a_meta.state = MAYBE_WINNING;
                }
                
                if (a_meta.state != WINNING && a_meta.ctrl_children == 0 && a_meta.state != MAYBE_WINNING)
                    a_meta.state = LOOSING;
                

            } else {
                assert(a_meta.state != MAYBE_LOSING);
                a_meta.env_children -= 1;
                if (next_meta.state == LOOSING) a_meta.state = LOOSING;
                if (next_meta.state == WINNING && a_meta.env_children == 0 && a_meta.ctrl_children == 0)
                    a_meta.state = WINNING;
            }

            if (a_meta.env_children == 0 && a_meta.state == MAYBE_WINNING) {
                a_meta.state = WINNING;
            }

            if (a_meta.state == WINNING || a_meta.state == LOOSING) {
                assert(store->get_meta(ancestor.second).state != PROCESSED);
                if (!a_meta.waiting) back.push(ancestor.second);
                a_meta.waiting = true;
            }
        }
        next_meta.dependers.clear();
    }

    void SafetySynthesis::successors(store_t::Pointer *parent,
                                     SafetyMeta &meta,
                                     waiting_t &waiting,
                                     bool is_controller,
                                     const Query* query) {

//        std::cout << (is_controller ? "controller" : "env ");
//        std::cout << " : " << *marking << std::endl;

        NonStrictMarkingBase *next = nullptr;

        std::vector<store_t::Pointer *> successors;
        size_t number_of_children = 0;
        bool terminated = false;
        bool all_loosing = true;
        bool some_winning = false;
        generator->reset();
        while ((next = generator->next(is_controller)) != nullptr) {

            largest = std::max(next->size(), largest);
            ++discovered;
            ++number_of_children;

//            std::cout << "\tchild  " << " : " << *next << std::endl;

            if (query->getQuantifier() == Quantifier::CG) {
                if (!satisfies_query(next)) {
    //            std::cout << "\t\tdoes not satisfy phi" << std::endl;
                    delete next;

                    if (is_controller) {
                        continue;
                    } else {
                        meta.state = LOOSING;
                        //std::cout << "LOOSING : " << parent << std::endl;
                        terminated = true;
                        break;
                    }
                }
            } else if (query->getQuantifier() == Quantifier::CF) {
                if (satisfies_query(next)) {
                    //std::cerr << "\t\tSAT!" << std::endl;
                    delete next;
                    if (!is_controller) {
                        //std::cerr << "NOT CTRL" << std::endl;
                        some_winning = true;
                        continue;
                    } else {
                        //std::cerr << "CTRL" << std::endl;
                        if(meta.state == MAYBE_LOSING)
                            meta.state = WINNING;
                        else
                            meta.state = MAYBE_WINNING;
                        terminated = true;
                        break;
                    }
                } 
            }
            else
            {
                std::cerr << "Using EG, EF, AG or AF without control is no longer supported" << std::endl;
                assert(false);
               std::exit(-1);
            }

            store_t::result_t res = store->insert_and_dealloc(next);

            store_t::Pointer *p = res.second;

            if (res.first) {
                //std::cerr << "\t\tNEW!" << std::endl;
                SafetyMeta childmeta = {UNKNOWN, false, 0, 0, depends_t()};
                store->set_meta(p, childmeta);
                successors.push_back(p);
                all_loosing = false;
//            std::cout << "\t\t" << p << std::endl;
                continue;
            }

            SafetyMeta &childmeta = store->get_meta(p);
            if (!is_controller && childmeta.state == LOOSING) {
                meta.state = LOOSING;
                //std::cout << "LOOSING2 : " << parent << std::endl;
                terminated = true;
                break;
            } else if (is_controller && (childmeta.state == WINNING || 
                    (p == parent && query->getQuantifier() == Quantifier::CG))) {
                if(meta.state == MAYBE_LOSING)
                    meta.state = WINNING;
                else
                    meta.state = MAYBE_WINNING;
                terminated = true;
                break;
            } else if (!is_controller && p == parent) {
                if(query->getQuantifier() == Quantifier::CF)
                {
                    meta.state = LOOSING;
                    //std::cout << "LOOSING3 : " << parent << std::endl;
                    terminated = true;
                    break;
                }
                else 
                    continue;
            }

            if (childmeta.state != WINNING && childmeta.state != LOOSING) {
                successors.push_back(p);
            }
            some_winning = some_winning || (childmeta.state == WINNING);
            all_loosing = all_loosing && (childmeta.state == LOOSING);
        }

        if (terminated)
        {
//            std::cerr << "TERMINATED" << std::endl;
            return; // Add nothing to waiting, we already have result
        }

        if (is_controller) {
            if(query->getQuantifier() == Quantifier::CG)
            {
                if (number_of_children == 0) {
                    meta.state = MAYBE_WINNING;
                    return;
                } else if (all_loosing) {
                    meta.state = LOOSING;
//                    std::cout << "LOOSING4 : " << parent << std::endl;
                    return;
                }
            }
            else if(query->getQuantifier() == Quantifier::CF)
            {
                if(number_of_children == 0 && meta.state == MAYBE_LOSING)
                {
                    meta.state = WINNING;
                    return;
                }
            }
        } else {
            if(query->getQuantifier() == Quantifier::CF)
            {
                // no unknown or losing successors, but some winning (and thus all winning)
                if(successors.size() == 0 && some_winning)
                {
                    meta.state = MAYBE_LOSING;
                    return;
                }
            }
        }

        std::sort(successors.begin(), successors.end());
        size_t unique = 0;
        bool first = true;
        store_t::Pointer *child = nullptr;
        for(auto p : successors) {
            if(p == child && !first) continue;
            else child = p;
            first = false;
            ++unique;

            SafetyMeta &childmeta = store->get_meta(child);
            childmeta.dependers.push_front(
                    depender_t(is_controller, parent));
            if (childmeta.state == UNKNOWN &&
                !childmeta.waiting) {
//                std::cerr << "ADDING TO WAITING" << std::endl;
                childmeta.waiting = true;
                waiting.push(child);
            }
        }

        if (is_controller) meta.ctrl_children = unique;
        else meta.env_children = unique;
    }

    void SafetySynthesis::print_stats() {
        std::cout << "  discovered markings:\t" << discovered << std::endl;
        std::cout << "  explored markings:\t" << explored << std::endl;
        std::cout << "  stored markings:\t" << store->size() << std::endl;
    }

    SafetySynthesis::~SafetySynthesis() {
        delete store;
    }
} }

