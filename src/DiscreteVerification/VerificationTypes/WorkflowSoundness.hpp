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
#include "Workflow.hpp"
#include "../DataStructures/WaitingList.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

class WorkflowSoundness : public Workflow{
public:
	WorkflowSoundness(TAPN::TimedArcPetriNet& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<NonStrictMarking>* waiting_list);
	virtual ~WorkflowSoundness();
	bool verify();
	void printStats(){
		std::cout << "  discovered markings:\t" << pwList->discoveredMarkings << std::endl;
		std::cout << "  explored markings:\t" << pwList->size()-pwList->explored() << std::endl;
		std::cout << "  stored markings:\t" << pwList->size() << std::endl;
	}

	inline unsigned int maxUsedTokens(){ return pwList->maxNumTokensInAnyMarking; };
protected:
	virtual bool addToPW(NonStrictMarking* m){
		return addToPW(m, tmpParent);
	};
	bool addToPW(NonStrictMarking* marking, NonStrictMarking* parent);
	bool checkForCoveredMarking(NonStrictMarking* marking);

public:
	virtual void getTrace();
	void printExecutionTime(ostream& stream){
		stream << "Minimum execution time: " << min_exec << endl;
	}
protected:
	PWListBase* pwList;
    vector<NonStrictMarking*>* final_set;
    int min_exec;
    unsigned int linearSweepTreshold;
};

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* NONSTRICTSEARCH_HPP_ */
