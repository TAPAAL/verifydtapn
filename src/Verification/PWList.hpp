#ifndef PWLIST_HPP_
#define PWLIST_HPP_

#include "google/sparse_hash_map"
#include <list>
#include "PassedWaitingList.hpp"
#include "Node.hpp"
#include "../Core/DiscretePart.hpp"
#include "WaitingList.hpp"

namespace VerifyTAPN {
	class SymMarking;
	class Node;

	class PWList : public PassedWaitingList {
	private:
		typedef std::list<Node*> NodeList;
		typedef google::sparse_hash_map<const DiscretePart*, NodeList, VerifyTAPN::hash, VerifyTAPN::eqdp > HashMap;
	public:
		explicit PWList(WaitingList* waitingList) : map(), exploredStates(0), discoveredStates(0), storedStates(0), waitingList(waitingList) {};
		virtual ~PWList();

	public: // inspectors
		virtual bool HasWaitingStates() const;
		virtual long long Size() const;

		virtual void Print() const;

	public: // modifiers
		virtual bool Add(const SymMarking& symMarking);
		virtual  SymMarking& GetNextUnexplored();
	private:
		HashMap map;
		long long exploredStates;
		long long discoveredStates;
		long long storedStates;
		WaitingList* waitingList;
	};
}

#endif /* PWLIST_HPP_ */
