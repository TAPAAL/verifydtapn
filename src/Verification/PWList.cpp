#include "PWList.hpp"

namespace VerifyTAPN {
	void PWList::Add(const SymMarking& symMarking)
	{
		const DiscretePart* dp = symMarking.GetDiscretePart();

		std::list<Node>& markings = map[dp];
		const dbm::dbm_t& newDBM = symMarking.Zone();
		for(std::list<Node>::iterator iter = markings.begin(); iter != markings.end(); ++iter){
			const dbm::dbm_t& other = iter->GetMarking().Zone();
			relation_t relation = newDBM.relation(other);
			if(relation == base_SUBSET){
				return;
			}else if(relation == base_SUPERSET){
				if(iter->GetColor() == WAITING){
					waitingList->DecrementActualSize();
				}

				iter->Recolor(COVERED);
			}
		}

		Node node(symMarking, WAITING);
		markings.push_back(node);
		waitingList->Add(&markings.back());
	}

	long long PWList::Size() const
	{
		return map.size();
	}

	bool PWList::HasWaitingStates() const
	{
		return waitingList->Size() > 0;
	}

	SymMarking& PWList::GetNextUnexplored()
	{
		return waitingList->Next()->GetMarking();
	}
}
