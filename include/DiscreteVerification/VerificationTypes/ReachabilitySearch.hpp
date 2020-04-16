/*
 * NonStrictSearch.hpp
 *
 *  Created on: 26/04/2012
 *      Author: MathiasGS
 */

#ifndef REACHABILITYSEARCH_HPP_
#define REACHABILITYSEARCH_HPP_

#include "../DataStructures/PWList.hpp"
#include "../../Core/TAPN/TAPN.hpp"
#include "../../Core/QueryParser/AST.hpp"
#include "../../Core/VerificationOptions.hpp"
#include "../../Core/TAPN/TimedPlace.hpp"
#include "../../Core/TAPN/TimedTransition.hpp"
#include "../../Core/TAPN/TimedInputArc.hpp"
#include "../../Core/TAPN/TransportArc.hpp"
#include "../../Core/TAPN/InhibitorArc.hpp"
#include "../../Core/TAPN/OutputArc.hpp"
#include "../QueryVisitor.hpp"
#include "../DataStructures/NonStrictMarking.hpp"
#include <stack>
#include "Verification.hpp"
#include "../DataStructures/WaitingList.hpp"
#include "AbstractNaiveVerification.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

template<typename S>
class ReachabilitySearch : public AbstractNaiveVerification<PWListBase,NonStrictMarking,S> {
public:
        ReachabilitySearch(TAPN::TimedArcPetriNet& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options)               
	: AbstractNaiveVerification<PWListBase,NonStrictMarking,S>(tapn, initialMarking, query, options, NULL){}
        
	ReachabilitySearch(TAPN::TimedArcPetriNet& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<NonStrictMarking*>* waiting_list)
        : AbstractNaiveVerification<PWListBase,NonStrictMarking,S>(tapn, initialMarking, query, options, new PWList(waiting_list, false)) {}
	virtual ~ReachabilitySearch() = default;
	
        bool run(){
            if(handleSuccessor(&this->initialMarking, NULL)){
                    return true;
            }

            //Main loop
            while(this->pwList->hasWaitingStates()){
                    NonStrictMarking& next_marking = *this->pwList->getNextUnexplored();
                    this->tmpParent = &next_marking;
                    this->trace.push(&next_marking);
                    validChildren = 0;

                    bool noDelay = false;
                    auto res = this->generateAndInsertSuccessors(next_marking);
                    if(res == ADDTOPW_RETURNED_TRUE){
                        return true;
                    }  else if (res == ADDTOPW_RETURNED_FALSE_URGENTENABLED) {
                        noDelay = true;
                    }

                    // Generate next markings
                    if(!noDelay && this->isDelayPossible(next_marking)){
                            NonStrictMarking* marking = new NonStrictMarking(next_marking);
                            marking->incrementAge();
                            marking->setGeneratedBy(NULL);
                            if(handleSuccessor(marking, &next_marking)){
                                return true;
                            }
                    }
                    deleteMarking(&next_marking);

            }

            return false;
        }

        virtual void deleteMarking(NonStrictMarking* m) {
            //dummy;
        };
        
protected:
	bool handleSuccessor(NonStrictMarking* marking, NonStrictMarking* parent)
        {
            marking->cut(this->placeStats);
            marking->setParent(parent);

            unsigned int size = marking->size();

            this->pwList->setMaxNumTokensIfGreater(size);

            if(size > this->options.getKBound()) {
                    delete marking;
                    return false;
            }

            if(this->pwList->add(marking)){
                    QueryVisitor<NonStrictMarking> checker(*marking, this->tapn);
                    BoolResult context;        
                    this->query->accept(checker, context);
                    if(context.value) {
                            this->lastMarking = marking;
                            return true;
                    } else {
                            deleteMarking(marking);
                            return false;
                    }
            } else {
                    delete marking;
            }
            return false;
        }

protected:
	int validChildren;
public:
	virtual void getTrace()
        {
            stack < NonStrictMarking*> printStack;
            this->generateTraceStack(this->lastMarking, &printStack);
            if(this->options.getXmlTrace()){
                    this->printXMLTrace(this->lastMarking, printStack, this->query, this->tapn);
            } else {
                    this->printHumanTrace(this->lastMarking, printStack, this->query->getQuantifier());
            }
        }
};

template<typename S>
class ReachabilitySearchPTrie : public ReachabilitySearch<S> {
public:
    ReachabilitySearchPTrie(TAPN::TimedArcPetriNet& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<ptriepointer_t<MetaData*> >* waiting_list) 
    : ReachabilitySearch<S>(tapn,initialMarking, query, options)
    {
        this->pwList = new PWListHybrid(tapn, waiting_list, options.getKBound(), tapn.getNumberOfPlaces(), tapn.getMaxConstant(), false, options.getTrace() != VerificationOptions::NO_TRACE);
    };
    
    virtual void deleteMarking(NonStrictMarking* m) {
        delete m;
    };
    
    virtual void getTrace()
    {
	stack < NonStrictMarking*> printStack;
        PWListHybrid* pwhlist = dynamic_cast<PWListHybrid*>(this->pwList);
        MetaDataWithTraceAndEncoding* next = pwhlist->parent;
        NonStrictMarking* last = this->lastMarking;
        printStack.push(this->lastMarking);
        while(next != NULL){
            NonStrictMarking* m = pwhlist->decode(next->ep);
            m->setGeneratedBy(next->generatedBy);
            last->setParent(m);
            last = m;
            printStack.push(m);
            next = next->parent;
        };
        this->printXMLTrace(this->lastMarking, printStack, this->query, this->tapn);
    }

};

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* NONSTRICTSEARCH_HPP_ */
