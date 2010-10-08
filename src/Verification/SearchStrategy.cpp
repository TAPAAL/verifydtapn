#include "SearchStrategy.hpp"

namespace VerifyTAPN
{
	bool DFS::Execute()
	{
		pwList->Add(initialMarking);

		while(pwList->HasWaitingStates()){
			const SymMarking& next = pwList->GetNextUnexplored();

			// test property
			// successors
			// add successors to pwList

		}

		return false;
	}
}
