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

	bool PWList::Add(const SymMarking& symMarking)
	{
		stats.discoveredStates++;
		const DiscretePart* dp = symMarking.GetDiscretePart();
		NodeList& markings = map[dp];
		const dbm::dbm_t& newDBM = symMarking.Zone();
		for(NodeList::iterator iter = markings.begin(); iter != markings.end(); ++iter){
			Node* currentNode = *iter;
			assert(eqdp()(dp, currentNode->GetMarking().GetDiscretePart()));
//			std::cout << currentNode->GetMarking() << std::endl;
//			std::cout << symMarking << std::endl;
			const dbm::dbm_t& other = currentNode->GetMarking().Zone();
			relation_t relation = newDBM.relation(other);
			if(relation == base_SUBSET){
				return false;
			}else if(relation == base_SUPERSET){
				if(currentNode->GetColor() == WAITING){
					waitingList->DecrementActualSize();
				}

				currentNode->Recolor(COVERED);
			}
		}
		stats.storedStates++;
		Node* node = new Node(symMarking, WAITING);
		markings.push_back(node);
		waitingList->Add(node);
		return true;
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
		stats.exploredStates++;
		return waitingList->Next()->GetMarking();
	}

	Stats PWList::GetStats() const
	{
		return stats;
	}

	void PWList::Print() const
	{
		std::cout << stats;
		std::cout << ", waitingList: " << waitingList->Size() << "/" << waitingList->SizeIncludingCovered();

	}
}
