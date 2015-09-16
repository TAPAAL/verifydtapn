/* 
 * File:   SafetySynthesis.cpp
 * Author: Peter G. Jensen
 * 
 * Created on 14 September 2015, 15:59
 */
#include <assert.h>
#include "SafetySynthesis.h"
#include "../DataStructures/SimpleMarkingStore.h"

namespace VerifyTAPN {
namespace DiscreteVerification {
    
SafetySynthesis::SafetySynthesis(   TAPN::TimedArcPetriNet& tapn, 
                                    NonStrictMarking& initialMarking,
                                    AST::Query* query, 
                                    VerificationOptions& options)
                                : tapn(tapn), initial_marking(initialMarking),
                                    query(query), options(options), 
                                    placeStats(tapn.getNumberOfPlaces()),
                                    generator(tapn), discovered(0)
{
    store = new SimpleMarkingStore<SafetyMeta>();
}

bool SafetySynthesis::satisfies_query(NonStrictMarkingBase* m)
{
    m->cut(placeStats);
    if(m->size() > options.getKBound()) return false;
    QueryVisitor<NonStrictMarkingBase> checker(*m, tapn);
    BoolResult context;        
    query->accept(checker, context);
    return context.value;
}

bool SafetySynthesis::run()
{
    stack_t waiting;
    
    if(!satisfies_query(&initial_marking)) return false;
    
    store_t::result_t m_0_res = store->insert_and_dealloc(&initial_marking);
    SafetyMeta meta = {false, false, false, 0, 0, depends_t()};
    meta.waiting = true;
    m_0_res.second->set_meta_data(meta);
    waiting.push(m_0_res.second);
        
    while(!waiting.empty() && !m_0_res.second->get_meta_data().loosing)
    {
        store_t::Pointer* next = waiting.top();
        waiting.pop();
        
        SafetyMeta& next_meta = next->get_meta_data();
        next_meta.waiting = false;
        
        if(next_meta.loosing == true)
        {
            dependers_to_waiting(next_meta, waiting);
        }
        else
        {

            assert(next_meta.loosing == false);
            if(!next_meta.processed)    // first time we see the marking out of waiting
            {
                // generate successors for environment
                if(!successors(next, next_meta, waiting, false))
                {
                    next_meta.loosing = true;
                    waiting.push(next);
                }
            }
            // if we are not yet loosing and all env successors have been explored
            if(!next_meta.loosing && next_meta.env_children == 0)
            {
                // generate successors for controller
                if(!successors(next, next_meta, waiting, true))
                {
                    next_meta.loosing = true;
                    waiting.push(next);
                }
            }

            next_meta.processed = true;
        }
    }
    return !m_0_res.second->get_meta_data().loosing;
}

void SafetySynthesis::dependers_to_waiting(SafetyMeta& next_meta, stack_t& waiting)
{
    for(auto ancestor : next_meta.dependers)
    {
        SafetyMeta& a_meta = ancestor->get_meta_data();

        if(a_meta.loosing) continue;

        if(a_meta.env_children > 0)
        {
            a_meta.loosing = true;
        }
        else
        {
            a_meta.ctrl_children -= 1;
            a_meta.loosing = (a_meta.ctrl_children == 0);
        }

        if(a_meta.loosing && !a_meta.waiting)
        {
            a_meta.waiting = true;
            waiting.push(ancestor);
        }
    }
    next_meta.dependers.clear();
}

bool SafetySynthesis::successors(   store_t::Pointer* parent, 
                                    SafetyMeta& meta, 
                                    stack_t& waiting,
                                    bool is_controller)
{
    NonStrictMarkingBase* marking = store->expand(parent);
    generator.from_marking(marking, 
            is_controller ? 
                Generator::CONTROLLABLE : Generator::ENVIRONMENT);
    
    bool all_loosing = true;
    
//    std::cout << (controller ? "controller" : "env ") << std::endl;
//    std::cout << *marking << std::endl;
    /**
     * TODO: Add all markings to a vector first, and THEN insert into passed.
     * For evn at least, as if just ONE is loosing, then there is no need to 
     * explore more!
     * Similar CANNOT be done for ctrl successors. Here we just need *some*
     * to not be loosing - which we only know fore sure when waiting is empty.
     */
    NonStrictMarkingBase* next = NULL;
    bool some_child = false;
    while(next = generator.next(is_controller))
    {  
        ++discovered;
        some_child = true;
        if(!satisfies_query(next)) 
        {
            delete next;
            
            if(is_controller) 
            {
                continue;
            }
            else return false;
        }

//        std::cout << "child : " << *next << std::endl;
        
        store_t::result_t res = store->insert_and_dealloc(next);
        store_t::Pointer* p = res.second;
        if(res.first)
        {
            SafetyMeta childmeta = {false, false, false, 0, 0, depends_t()};
            p->set_meta_data(childmeta);
            waiting.push(p);
        }
               
        SafetyMeta& childmeta = p->get_meta_data();
        if(!childmeta.loosing)
        {
            all_loosing = false;
            if(is_controller) meta.ctrl_children += 1;
            else meta.env_children += 1;
            
            childmeta.dependers.push_front(parent);
            continue;
        }
        else if(!is_controller) return false;    
    }
    
    if(is_controller)
    {
        if(!some_child) return true;
        else return !all_loosing;
    }
    else return true;
}

void SafetySynthesis::print_stats() {
    std::cout << "  discovered markings:\t" << discovered << std::endl;
    std::cout << "  explored markings:\t" << 0 << std::endl;
    std::cout << "  stored markings:\t" << store->size() << std::endl;
};

SafetySynthesis::~SafetySynthesis() {
    delete store;
}
}
}

