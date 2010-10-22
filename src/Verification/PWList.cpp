#include "PWList.hpp"

namespace VerifyTAPN {
	PWList::~PWList()
	{
		 delete waitingList;
		 for(HashMap::iterator iter = map.begin(); iter != map.end(); ++iter){
			NodeList& list = (*iter).second;

			 for(NodeList::iterator iter = list.begin(); iter != list.end(); ++iter){
				 Node* node = *iter;
				 delete node;
			 }
		 }
	}

	void PWList::Add(const SymMarking& symMarking)
	{
		const DiscretePart* dp = symMarking.GetDiscretePart();
		NodeList& markings = map[dp];
		const dbm::dbm_t& newDBM = symMarking.Zone();
		for(NodeList::iterator iter = markings.begin(); iter != markings.end(); ++iter){
			Node* currentNode = *iter;
			const dbm::dbm_t& other = currentNode->GetMarking().Zone();
			relation_t relation = newDBM.relation(other);
			if(relation == base_SUBSET){
				return;
			}else if(relation == base_SUPERSET){
				if(currentNode->GetColor() == WAITING){
					waitingList->DecrementActualSize();
				}

				currentNode->Recolor(COVERED);
			}
		}

		Node* node = new Node(symMarking, WAITING);
		markings.push_back(node);
		waitingList->Add(node);
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
