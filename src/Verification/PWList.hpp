#ifndef PWLIST_HPP_
#define PWLIST_HPP_

#include "google/sparse_hash_map"
#include <list>
#include "../Core/SymMarking.hpp"
#include "PassedWaitingList.hpp"
#include "WaitingList.hpp"
#include "Node.hpp"


namespace VerifyTAPN {
	class PWList : public PassedWaitingList {
	private:
		typedef google::sparse_hash_map<const DiscretePart*, std::list<Node> > HashMap;
	public:
		explicit PWList(WaitingList* waitingList) : map(), waitingList(waitingList) {};
		virtual ~PWList() { delete waitingList; };

	public: // inspectors
		virtual bool IsEmpty() const;
		virtual long long Size() const;
	public: // modifiers
		virtual void Add(const SymMarking& symMarking);

	private:
		HashMap map;
		WaitingList* waitingList;
	};
}

#endif /* PWLIST_HPP_ */
