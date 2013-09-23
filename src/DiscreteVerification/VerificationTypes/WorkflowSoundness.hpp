/*
 * NonStrictSearch.hpp
 *
 *  Created on: 26/04/2012
 *      Author: MathiasGS
 */

#ifndef WORKFLOWSOUNDNESS_HPP_
#define WORKFLOWSOUNDNESS_HPP_

#include "../DataStructures/WorkflowPWList.hpp"
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

class WorkflowSoundness : public Verification<NonStrictMarking>{
public:
	enum ModelType{
		MTAWFN, ETAWFN, NOTTAWFN
	};
public:
    WorkflowSoundness(TAPN::TimedArcPetriNet& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options);
	WorkflowSoundness(TAPN::TimedArcPetriNet& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<NonStrictMarking>* waiting_list);
	virtual ~WorkflowSoundness();
	bool verify();
	inline unsigned int maxUsedTokens(){ return pwList->maxNumTokensInAnyMarking; };
	void printTransitionStatistics() const { successorGenerator.printTransitionStatistics(std::cout); }
        virtual void deleteMarking(NonStrictMarking* m) {
            //dummy;
        };
         virtual bool addToPW(NonStrictMarking* m){
            return addToPW(m, tmpParent);
        };
    inline int getMinExecutionTime(){	return min_exec;	}
protected:
	bool addToPW(NonStrictMarking* marking, NonStrictMarking* parent);
	bool isDelayPossible(NonStrictMarking& marking);
	ModelType calculateModelType();
	bool checkForCoveredMarking(NonStrictMarking* marking);

protected:
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
	inline const ModelType getModelType() const{ return modelType; }
protected:
    NonStrictMarking* tmpParent;
    TimedPlace* in;
    TimedPlace* out;
    ModelType modelType;
    vector<NonStrictMarking*>* verify_set;
    vector<NonStrictMarking*>* final_set;
    NonStrictMarking* lastMarking;
    int min_exec;
};

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* NONSTRICTSEARCH_HPP_ */
