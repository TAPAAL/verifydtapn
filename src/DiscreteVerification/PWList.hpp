/*
 * PWList.hpp
 *
 *  Created on: 01/03/2012
 *      Author: MathiasGS
 */

#ifndef PWLIST_HPP_
#define PWLIST_HPP_

#include "WaitingList.hpp"

#include "google/sparse_hash_map"

namespace VerifyTAPN {
namespace DiscreteVerification {

class PWList {
public:
	typedef google::sparse_hash_map<size_t, NonStrictMarking> HashMap;
public:
	PWList();
	virtual ~PWList();

public: // inspectors
	virtual bool HasWaitingStates() const {
		//TODO: implement!
		return false;
	};

	virtual long long Size() const {
		//TODO: implement!
		return 0;
	};

	virtual void Print() const {
		//TODO: implement!
	};

public: // modifiers
	virtual void Add(const NonStrictMarking& marking);
	virtual const NonStrictMarking& GetNextUnexplored();

private:
	HashMap markings_storage;
	StackWaitingList waiting_list;
};

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* PWLIST_HPP_ */
