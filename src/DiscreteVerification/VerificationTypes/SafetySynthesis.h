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


namespace VerifyTAPN {
namespace DiscreteVerification {


class SafetySynthesis {
private:
    
    struct SafetyMeta {
        bool loosing;
        bool waiting;
        std::forward_list<SafetyMeta> dependers;        
    };
    
    
    MarkingStore<SafetyMeta>* store;
    TAPN::TimedArcPetriNet& tapn;
    NonStrictMarking& initial_marking;
    AST::Query* query;
    VerificationOptions options;
public:
    SafetySynthesis(
            TAPN::TimedArcPetriNet& tapn, 
            NonStrictMarking& initialMarking,
            AST::Query* query, 
            VerificationOptions& options);
    
    bool run();
    
    virtual ~SafetySynthesis();
private:

};
}
}

#endif	/* SAFETYSYNTHESIS_H */

