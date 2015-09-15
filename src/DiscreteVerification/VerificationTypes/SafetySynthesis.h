/* 
 * File:   SafetySynthesis.h
 * Author: Peter G. Jensen
 *
 * Created on 14 September 2015, 15:59
 */

#ifndef SAFETYSYNTHESIS_H
#define	SAFETYSYNTHESIS_H

#include <forward_list>

#include "../DataStructures/MarkingStore.h"
#include "../../Core/TAPN/TAPN.hpp"
#include "../SuccessorGenerator.hpp"
#include "../QueryVisitor.hpp"
#include "../DataStructures/NonStrictMarkingBase.hpp"
#include "../Generator.h"


namespace VerifyTAPN {
namespace DiscreteVerification {


class SafetySynthesis {
private:
    struct SafetyMeta;
    typedef MarkingStore<SafetyMeta> store_t;
    typedef std::stack<store_t::Pointer*> stack_t;
    
    typedef std::forward_list<store_t::Pointer*> depends_t;
    
    struct SafetyMeta {
        bool loosing;
        bool waiting;
        bool processed;
        size_t env_children;
        size_t ctrl_children;
        depends_t dependers;        
    };
    

    
    store_t* store;
    TAPN::TimedArcPetriNet& tapn;
    NonStrictMarking& initial_marking;
    AST::Query* query;
    VerificationOptions options;
    std::vector<int> placeStats;
    Generator generator;
public:
    SafetySynthesis(
            TAPN::TimedArcPetriNet& tapn, 
            NonStrictMarking& initialMarking,
            AST::Query* query, 
            VerificationOptions& options);
    
    bool run();
    
    virtual ~SafetySynthesis();
private:
    bool satisfies_query(NonStrictMarkingBase* m);
    bool ctrl_successors(MarkingStore<SafetyMeta>::Pointer*, SafetyMeta&, stack_t& waiting);
    bool env_successors(MarkingStore<SafetyMeta>::Pointer*, SafetyMeta&, stack_t& waiting);
};
}
}

#endif	/* SAFETYSYNTHESIS_H */

