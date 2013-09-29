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
#include "../SuccessorGenerator.hpp"
#include "../QueryVisitor.hpp"
#include "../DataStructures/NonStrictMarking.hpp"
#include <stack>
#include "Verification.hpp"
#include "../DataStructures/WaitingList.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

class ReachabilitySearch : public Verification<NonStrictMarking>{
public:
        ReachabilitySearch(TAPN::TimedArcPetriNet& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options);
	ReachabilitySearch(TAPN::TimedArcPetriNet& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<NonStrictMarking>* waiting_list);
	virtual ~ReachabilitySearch();
	bool verify();
	NonStrictMarking* getLastMarking() { return lastMarking; }
	inline unsigned int maxUsedTokens(){ return pwList->maxNumTokensInAnyMarking; };
	void printTransitionStatistics() const { successorGenerator.printTransitionStatistics(std::cout); }
        virtual void deleteMarking(NonStrictMarking* m) {
            //dummy;
        };
         virtual bool addToPW(NonStrictMarking* m){
            return addToPW(m, tmpParent);
        };
protected:
	bool addToPW(NonStrictMarking* marking, NonStrictMarking* parent);
	bool isDelayPossible(NonStrictMarking& marking);

public:
	int validChildren;
	PWListBase* pwList;
	TAPN::TimedArcPetriNet& tapn;
	NonStrictMarking& initialMarking;
	AST::Query* query;
	VerificationOptions options;
	SuccessorGenerator<NonStrictMarking> successorGenerator;
public:
	void printStats();
	virtual void getTrace();
protected:
	NonStrictMarking* lastMarking;
    NonStrictMarking* tmpParent;
};

class ReachabilitySearchPTrie : public ReachabilitySearch{
public:
    ReachabilitySearchPTrie(TAPN::TimedArcPetriNet& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<EncodingPointer<MetaData> >* waiting_list) 
    : ReachabilitySearch(tapn,initialMarking, query, options)
    {
        pwList = new PWListHybrid(tapn, waiting_list, options.getKBound(), tapn.getNumberOfPlaces(), tapn.getMaxConstant(), false, options.getTrace() == VerificationOptions::SOME_TRACE);
    };
    
    virtual void deleteMarking(NonStrictMarking* m) {
        delete m;
    };
    
   virtual void getTrace();

};

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* NONSTRICTSEARCH_HPP_ */
