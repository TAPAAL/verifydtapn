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
		typedef google::sparse_hash_map<const DiscretePart*, std::list<Node>, VerifyTAPN::hash > HashMap;
	public:
		explicit PWList(WaitingList* waitingList) : map(), waitingList(waitingList) {};
		virtual ~PWList() { delete waitingList; };

	public: // inspectors
		virtual bool HasWaitingStates() const;
		virtual long long Size() const;
	public: // modifiers
		virtual void Add(const SymMarking& symMarking);
		virtual const SymMarking& GetNextUnexplored();
	private:
		HashMap map;
		WaitingList* waitingList;
	};
}

#endif /* PWLIST_HPP_ */
