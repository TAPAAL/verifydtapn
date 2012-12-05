#ifndef TIMEDARTVERIFICATION_HPP_
#define TIMEDARTVERIFICATION_HPP_

#include "../../Core/TAPN/TAPN.hpp"
#include "../DataStructures/NonStrictMarking.hpp"
#include "../Util/IntervalOps.hpp"
#include "../TimeDartSuccessorGenerator.hpp"
#include "Verification.hpp"
#include "../DataStructures/TimeDart.hpp"
#include <stack>

namespace VerifyTAPN {
namespace DiscreteVerification {

using namespace rapidxml;

typedef pair<NonStrictMarking*, int> TraceList;

class TimeDartVerification : public Verification<NonStrictMarking> {
public:

	TimeDartVerification(boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn, VerificationOptions options, AST::Query* query, NonStrictMarking& initialMarking):
		query(query), options(options), tapn(tapn), initialMarking(initialMarking), exploredMarkings(0), allwaysEnabled(), successorGenerator(*tapn.get()){

		//Find the transitions which don't have input arcs
		for(TimedTransition::Vector::const_iterator iter = tapn->GetTransitions().begin(); iter != tapn->GetTransitions().end(); iter++){
			if((*iter)->GetPreset().size() + (*iter)->GetTransportArcs().size() == 0){
				allwaysEnabled.push_back(iter->get());
			}
		}
	}

	std::pair<int, int> calculateStart(const TAPN::TimedTransition& transition, NonStrictMarking* marking);
	int calculateStop(const TAPN::TimedTransition& transition, NonStrictMarking* marking);
	int maxPossibleDelay(NonStrictMarking* marking);
	vector<NonStrictMarking*> getPossibleNextMarkings(NonStrictMarking& marking, const TimedTransition& transition);
	void PrintXMLTrace(TraceList* m, std::stack<TraceList*>& stack, Quantifier query);
	void PrintTransitionStatistics() const { successorGenerator.PrintTransitionStatistics(std::cout); }

protected:
	AST::Query* query;
	VerificationOptions options;
	boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn;
	NonStrictMarking& initialMarking;
	int exploredMarkings;
	vector<const TAPN::TimedTransition*> allwaysEnabled;

private:
	TimeDartSuccessorGenerator successorGenerator;

};
}
}

#endif
