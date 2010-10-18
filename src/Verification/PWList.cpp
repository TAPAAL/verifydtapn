#include "PWList.hpp"

namespace VerifyTAPN {
	void PWList::Add(const SymMarking& symMarking)
	{
		const DiscretePart* dp = symMarking.GetDiscretePart();
		Node node(symMarking, WAITING);

		std::list<Node>& markings = map[dp];
		const dbm::dbm_t& newDBM = symMarking.Zone();
		for(std::list<Node>::const_iterator iter = markings.begin(); iter != markings.end(); ++iter){
			const dbm::dbm_t& other = iter->GetMarking().Zone();
			relation_t relation = newDBM.relation(other);
			if(relation == base_SUBSET){
				return;
			}else if(relation == base_SUPERSET){
				// swap markings or similar (but there may be others covered by newDBM?)
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
