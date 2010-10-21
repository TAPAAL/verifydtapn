#include "SearchStrategy.hpp"

namespace VerifyTAPN
{
	DFS::DFS(
		const VerifyTAPN::TAPN::TimedArcPetriNet& tapn,
		const SymMarking& initialMarking,
		const AST::Query* query,
		const VerificationOptions& options
	) : tapn(tapn), initialMarking(initialMarking), checker(query), options(options)
	{
		pwList = new PWList(new StackWaitingList);
	};

	bool DFS::Execute()
	{
		pwList->Add(initialMarking);
		if(CheckQuery(initialMarking)) return checker.IsEF(); // return true if EF query (proof found), or false if AG query (counter example found)

		while(pwList->HasWaitingStates()){
			SymMarking& next = pwList->GetNextUnexplored();

			next.Delay();

			typedef std::vector<SymMarking*> SuccessorVector;
			SuccessorVector successors;

			next.GenerateDiscreteTransitionSuccessors(tapn, options.GetKBound(), successors);

			for(SuccessorVector::iterator iter = successors.begin(); iter != successors.end(); ++iter)
			{
				if(CheckQuery(**iter)) return checker.IsEF();
				pwList->Add(**iter);
			}
		}

		return checker.IsAG(); // return true if AG query (no counter example found), false if EF query (no proof found)
	}

	bool DFS::CheckQuery(const SymMarking& marking) const
	{
		bool satisfied = checker.IsExpressionSatisfied(marking);
		return (satisfied && checker.IsEF()) || (!satisfied && checker.IsAG());
	}
}
