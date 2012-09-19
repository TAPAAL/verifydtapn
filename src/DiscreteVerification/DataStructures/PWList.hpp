/*
 * PWList.hpp
 *
 *  Created on: 01/03/2012
 *      Author: MathiasGS
 */

#ifndef PWLIST_HPP_
#define PWLIST_HPP_

#include "WaitingList.hpp"
#include <iostream>
#include "google/sparse_hash_map"
#include  "NonStrictMarking.hpp"
#include "NonStrictMarking.hpp"
#include "WaitingList.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {
class PWList {
public:
	typedef std::vector<NonStrictMarking*> NonStrictMarkingList;
	typedef google::sparse_hash_map<size_t, NonStrictMarkingList> HashMap;
public:
	PWList() : markings_storage(256000), waiting_list(), discoveredMarkings(0), maxNumTokensInAnyMarking(-1) {};
	PWList(WaitingList<NonStrictMarking>* w_l) : markings_storage(256000), waiting_list(w_l), discoveredMarkings(0), maxNumTokensInAnyMarking(-1) {};
	virtual ~PWList();
	friend std::ostream& operator<<(std::ostream& out, PWList& x);

public: // inspectors
	virtual bool HasWaitingStates() {
		return (waiting_list->Size() > 0);
	};

	virtual long long Size() const {
		return markings_storage.size();
	};

public: // modifiers
	virtual bool Add(NonStrictMarking* marking);
	virtual NonStrictMarking* GetNextUnexplored();
	inline void SetMaxNumTokensIfGreater(int i){ if(i>maxNumTokensInAnyMarking) maxNumTokensInAnyMarking = i; };

public:
	HashMap markings_storage;
	WaitingList<NonStrictMarking>* waiting_list;
	int discoveredMarkings;
	int maxNumTokensInAnyMarking;
};

std::ostream& operator<<(std::ostream& out, PWList& x);

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* PWLIST_HPP_ */
