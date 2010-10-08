#include "PWList.hpp"

namespace VerifyTAPN {
	void PWList::Add(const SymMarking& symMarking)
	{
		const DiscretePart* dp = symMarking.GetDiscretePart();
		Node node(symMarking, WAITING);

		std::list<Node>& markings = map[dp];
		for(std::list<Node>::const_iterator iter = markings.begin(); iter != markings.end(); ++iter){
			const dbm::dbm_t& newDBM = symMarking.GetZone();
			const dbm::dbm_t& other = iter->GetMarking().GetZone();
			if(newDBM.relation(other) == base_SUBSET){
				return;
			}
		}
		markings.push_back(node);
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
