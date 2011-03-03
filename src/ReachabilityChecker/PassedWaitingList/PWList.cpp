#include "PWList.hpp"
#include "../../Core/SymbolicMarking/MarkingFactory.hpp"
#include "../../Core/SymbolicMarking/DiscreteMarking.hpp"

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

	bool PWList::Add(const SymbolicMarking& symMarking)
	{
		StoredMarking* storedMarking = factory->Convert(const_cast<SymbolicMarking*>(&symMarking));
		stats.discoveredStates++;
		//const DiscretePart& dp = symMarking.GetDiscretePart();
		NodeList& markings = map[storedMarking->HashKey()];
		NodeList::iterator iter = markings.begin();

		while(iter != markings.end())
		{
			Node* currentNode 		= *iter;
			relation relation 	= storedMarking->Relation(*currentNode->GetMarking());
			//assert(eqdp()(currentNode->GetMarking().GetDiscretePart(), dp));
			if((relation & SUBSET) != 0)
			{ // check subseteq
				return false;
			}
			else if(relation == SUPERSET)
			{
				assert(currentNode->GetColor() != COVERED);
				if(currentNode->GetColor() == WAITING)
				{
					waitingList->DecrementActualSize();
					currentNode->Recolor(COVERED);
				}
				else
				{
					delete currentNode;
				}
				iter = markings.erase(iter);
				stats.storedStates--;
				continue;
			}
			iter++;
		}

		stats.storedStates++;
		Node* node = new Node(storedMarking, WAITING);
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

	SymbolicMarking* PWList::GetNextUnexplored()
	{
		stats.exploredStates++;
		StoredMarking* next = waitingList->Next()->GetMarking();
		return factory->Convert(next);
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
