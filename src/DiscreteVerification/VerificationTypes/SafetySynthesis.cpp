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
                                    generator(tapn)
{
    store = new SimpleMarkingStore<SafetyMeta>();
}

bool SafetySynthesis::satisfies_query(NonStrictMarkingBase* m)
{
    QueryVisitor<NonStrictMarkingBase> checker(*m, tapn);
    BoolResult context;        
    query->accept(checker, context);
    return context.value;
}

bool SafetySynthesis::run()
{
    stack_t waiting;
    
    initial_marking.cut(placeStats);
    
    if(!satisfies_query(&initial_marking)) return false;
    
    store_t::result_t m_0_res = store->insert_and_dealloc(&initial_marking);
    SafetyMeta meta = {
                false, 
                false, 
                false, 
                0,
                0,
                depends_t()};
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
            for(depends_t::iterator it = next_meta.dependers.begin();
                    it != next_meta.dependers.end(); ++it)
            {
                store_t::Pointer* ancestor = *it;
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
                    waiting.push(*it);
                }
            }
            next_meta.dependers.clear();
        }
        else
        {

            assert(next_meta.loosing == false);
            if(!next_meta.processed)
            {
                // generate successors for environment
                if(!env_successors(next, next_meta, waiting))
                {
                    next_meta.loosing = true;
                    waiting.push(next);
                }
            }

            if(!next_meta.loosing && next_meta.env_children == 0)
            {
                // generate successors for controller
                if(!ctrl_successors(next, next_meta, waiting))
                {
                    next_meta.loosing = true;
                    waiting.push(next);
                }
            }

            next_meta.processed = true;
        }
    }
    return false;
}

bool SafetySynthesis::env_successors(   store_t::Pointer* parent, 
                                        SafetyMeta& meta, 
                                        stack_t& waiting)
{
    NonStrictMarkingBase* marking = store->expand(parent);
    generator.from_marking(marking, Generator::CONTROLLABLE);
    std::cout << *marking << std::endl;
    while(NonStrictMarkingBase* next = generator.next())
    {        
        std::cout << *next << std::endl;
        next->cut(placeStats);
        
        if(next->size() > options.getKBound() ||
                !satisfies_query(next)) 
        {
            delete next;
            return false;
        }
        
        store_t::result_t res = store->insert_and_dealloc(next);
        store_t::Pointer* p = res.second;
        if(res.first)
        {
            SafetyMeta childmeta = {
                false, 
                false, 
                false, 
                0,
                0,
                depends_t()};
            
            childmeta.dependers.push_front(parent);
            p->set_meta_data(childmeta);            
            meta.env_children += 1;
            meta.waiting = true;
            waiting.push(res.second);
        }
        else
        {
            SafetyMeta& childmeta = p->get_meta_data();
            if(!childmeta.loosing)
            {
                meta.env_children += 1;
                childmeta.dependers.push_front(parent);
                continue;
            }
            else
            {
                return false;
            }
        }        
    }
    return true;
}

bool SafetySynthesis::ctrl_successors(  store_t::Pointer* parent, 
                                        SafetyMeta& meta, 
                                        stack_t& waiting)
{
    bool all_loosing = true;
    bool some_child = false;
    NonStrictMarkingBase* marking = store->expand(parent);
    generator.from_marking(marking, Generator::CONTROLLABLE);
    while(NonStrictMarkingBase* next = generator.next())
    {
        some_child = true;
        next->cut(placeStats);
        if(next->size() > options.getKBound() ||
                !satisfies_query(next)) 
        {
            delete next;
        }
        
        store_t::result_t res = store->insert_and_dealloc(next);
        store_t::Pointer* p = res.second;
        if(res.first)
        {
            SafetyMeta childmeta = {
                false, 
                false, 
                false, 
                0,
                0,
                depends_t()};
            
            childmeta.dependers.push_front(parent);
            p->set_meta_data(childmeta);            
            meta.ctrl_children += 1;
            meta.waiting = true;
            waiting.push(res.second);
            all_loosing = false;
        }
        else
        {
            SafetyMeta& childmeta = p->get_meta_data();
            if(!childmeta.loosing)
            {
                meta.ctrl_children += 1;
                childmeta.dependers.push_front(parent);
                all_loosing = false;
            }
        }        
    }
    if(!some_child) return true;
    else return all_loosing;
}

SafetySynthesis::~SafetySynthesis() {
    delete store;
}
}
}

