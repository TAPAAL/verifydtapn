#include "PWList.hpp"

namespace VerifyTAPN {
	void PWList::Add(const SymMarking& symMarking)
	{
		const DiscretePart* dp = symMarking.GetDiscretePart();
		Node node(symMarking, WAITING);
		map[dp].push_back(node);
		waitingList->Add(&node);
	}

	long long PWList::Size() const
	{
		return map.size();
	}

	bool PWList::HasWaitingStates() const
	{
		return waitingList->Size() > 0;
	}

	const SymMarking& PWList::GetNextUnexplored()
	{
		return waitingList->Next()->GetMarking();
	}
}
