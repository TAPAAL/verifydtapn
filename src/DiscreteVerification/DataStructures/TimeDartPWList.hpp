/*
 * PWList.hpp
 *
 *  Created on: 01/03/2012
 *      Author: MathiasGS
 */

#ifndef TIMEDARTPWLIST_HPP_
#define TIMEDARTPWLIST_HPP_

#include "WaitingList.hpp"
#include <iostream>
#include "google/sparse_hash_map"
#include  "NonStrictMarking.hpp"
#include "NonStrictMarking.hpp"
#include "WaitingList.hpp"
#include "TimeDart.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {
class TimeDartPWList {
public:
	typedef std::vector<TimeDart*> NonStrictMarkingList;
	typedef google::sparse_hash_map<size_t, NonStrictMarkingList> HashMap;
public:
	TimeDartPWList() : markings_storage(256000), waiting_list(), discoveredMarkings(0), maxNumTokensInAnyMarking(-1) {};
	TimeDartPWList(WaitingList<TimeDart>* w_l) : markings_storage(256000), waiting_list(w_l), discoveredMarkings(0), maxNumTokensInAnyMarking(-1) {};
	virtual ~TimeDartPWList();
	friend std::ostream& operator<<(std::ostream& out, TimeDartPWList& x);

public: // inspectors
	virtual bool HasWaitingStates() {
		return (waiting_list->Size() > 0);
	};

	virtual long long Size() const {
		return markings_storage.size();
	};

public: // modifiers
	virtual bool Add(TAPN::TimedArcPetriNet* tapn, NonStrictMarking* marking, int w, int p);
	virtual TimeDart* GetNextUnexplored();
	inline void SetMaxNumTokensIfGreater(int i){ if(i>maxNumTokensInAnyMarking) maxNumTokensInAnyMarking = i; };

public:
	HashMap markings_storage;
	WaitingList<TimeDart>* waiting_list;
	int discoveredMarkings;
	int maxNumTokensInAnyMarking;
};

std::ostream& operator<<(std::ostream& out, TimeDartPWList& x);

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* PWLIST_HPP_ */
