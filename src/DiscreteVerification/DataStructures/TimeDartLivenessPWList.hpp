/*
 * PWList.hpp
 *
 *  Created on: 01/03/2012
 *      Author: MathiasGS
 */

#ifndef TimeDartLivenessPWList_HPP_
#define TimeDartLivenessPWList_HPP_

#include "WaitingList.hpp"
#include <iostream>
#include "google/sparse_hash_map"
#include "NonStrictMarkingBase.hpp"
#include "WaitingList.hpp"
#include "TimeDart.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {
class TimeDartLivenessPWList {
public:
	typedef std::vector<TimeDart*> TimeDartList;
	typedef google::sparse_hash_map<size_t, TimeDartList> HashMap;
public:
	TimeDartLivenessPWList() : markings_storage(256000), waiting_list(), discoveredMarkings(0), maxNumTokensInAnyMarking(-1) {};
	TimeDartLivenessPWList(WaitingList<WaitingDart>* w_l) : markings_storage(256000), waiting_list(w_l), discoveredMarkings(0), maxNumTokensInAnyMarking(-1) {};
	virtual ~TimeDartLivenessPWList();
	friend std::ostream& operator<<(std::ostream& out, TimeDartLivenessPWList& x);

public: // inspectors
	virtual bool HasWaitingStates() {
		return (waiting_list->Size() > 0);
	};

	virtual long long Size() const {
		return markings_storage.size();
	};

public: // modifiers
	virtual std::pair<TimeDart*, bool> Add(TAPN::TimedArcPetriNet* tapn, NonStrictMarkingBase* base, int youngest, TimeDart* parent, int upper);
	virtual WaitingDart* GetNextUnexplored();
	virtual WaitingDart* PopWaiting();
	virtual void flushBuffer();
	inline void SetMaxNumTokensIfGreater(int i){ if(i>maxNumTokensInAnyMarking) maxNumTokensInAnyMarking = i; };

public:
	HashMap markings_storage;
	WaitingList<WaitingDart>* waiting_list;
	int discoveredMarkings;
	int maxNumTokensInAnyMarking;
};

std::ostream& operator<<(std::ostream& out, TimeDartLivenessPWList& x);

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* PWLIST_HPP_ */
