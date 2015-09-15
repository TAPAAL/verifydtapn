/* 
 * File:   SafetySynthesis.cpp
 * Author: Peter G. Jensen
 * 
 * Created on 14 September 2015, 15:59
 */

#include "SafetySynthesis.h"
#include "../DataStructures/SimpleMarkingStore.h"
namespace VerifyTAPN {
namespace DiscreteVerification {
    
SafetySynthesis::SafetySynthesis(   TAPN::TimedArcPetriNet& tapn, 
                                    NonStrictMarking& initialMarking,
                                    AST::Query* query, 
                                    VerificationOptions& options)
                                : tapn(tapn), initial_marking(initialMarking),
                                    query(query), options(options)
{
    store = new SimpleMarkingStore<SafetyMeta>();
}

bool SafetySynthesis::run()
{
    return false;
}

SafetySynthesis::~SafetySynthesis() {
    delete store;
}
}
}

