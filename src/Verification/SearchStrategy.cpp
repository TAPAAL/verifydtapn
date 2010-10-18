#include "SearchStrategy.hpp"

namespace VerifyTAPN
{
	DFS::DFS(
		const VerifyTAPN::TAPN::TimedArcPetriNet& tapn,
		const SymMarking& initialMarking,
		const AST::Query* query
	) : tapn(tapn), initialMarking(initialMarking), checker(query)
	{
		pwList = new PWList(new StackWaitingList);
	};

	bool DFS::Execute()
	{
		pwList->Add(initialMarking);
		if(CheckQuery(initialMarking)) return checker.IsEF(); // return true if EF query, or false if AG query (counter example found)

		while(pwList->HasWaitingStates()){
			const SymMarking& next = pwList->GetNextUnexplored();

			//next.Delay();

			std::vector<SymMarking*> successors;

			//if(CheckQuery(succ)) return checker.IsEF();
			// successors
			// add successors to pwList

		}

		return false;
	}

	bool DFS::CheckQuery(const SymMarking& marking) const
	{
		bool satisfied = checker.IsExpressionSatisfied(marking);
		return (satisfied && checker.IsEF()) || (!satisfied && checker.IsAG());
	}
}
