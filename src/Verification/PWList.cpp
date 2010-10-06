#include "PWList.hpp"

namespace VerifyTAPN {
	void PWList::Add(const SymMarking& symMarking)
	{
		const DiscretePart* dp = symMarking.GetDiscretePart();
		Node node(symMarking, WAITING);
		map[dp].push_back(node);
	}

	long long PWList::Size() const
	{
		return map.size();
	}

	bool PWList::IsEmpty() const
	{
		return Size() == 0;
	}
}
