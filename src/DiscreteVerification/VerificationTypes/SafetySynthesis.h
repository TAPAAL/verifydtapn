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
            
    enum MarkingState {
                        UNKNOWN,            // no successors generated yet
                        MAYBE_LOOSING,      // Generated (some) successors for env
                        LOOSING,            // env wins
                        MAYBE_WINNING,      // Generated (some) successors for ctrl
                        WINNING};           // ctrl surely wins
    
    struct SafetyMeta {
        MarkingState state;
        bool waiting;                       // We only need stuff on waiting once
        size_t children;                    // Usefull.
        depends_t dependers;                // A punch of parents       
    };
    

    
    store_t* store;
    TAPN::TimedArcPetriNet& tapn;
    NonStrictMarking& initial_marking;
    AST::Query* query;
    VerificationOptions options;
    std::vector<int> placeStats;
    Generator generator;
    size_t discovered;
    size_t explored;
    unsigned int largest;
public:
    SafetySynthesis(
            TAPN::TimedArcPetriNet& tapn, 
            NonStrictMarking& initialMarking,
            AST::Query* query, 
            VerificationOptions& options);
    
    bool run();
    
    virtual ~SafetySynthesis();
    unsigned int max_tokens() { return largest; };
    void print_stats();
private:
    bool satisfies_query(NonStrictMarkingBase* m);
    void successors(MarkingStore<SafetyMeta>::Pointer*, SafetyMeta&, 
                                            stack_t& waiting, bool controller);
    void dependers_to_waiting(SafetyMeta& next_meta, stack_t& waiting);
};
}
}

#endif	/* SAFETYSYNTHESIS_H */

