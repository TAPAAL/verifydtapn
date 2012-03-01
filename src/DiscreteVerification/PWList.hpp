/*
 * PWList.hpp
 *
 *  Created on: 01/03/2012
 *      Author: MathiasGS
 */

#ifndef PWLIST_HPP_
#define PWLIST_HPP_

#import "WaitingList.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

class PWList {
public:
	typedef google::sparse_hash_map<size_t, NonStrictMarking> HashMap;
public:
	PWList();
	virtual ~PWList();

public: // inspectors
	virtual bool HasWaitingStates() const;
	virtual long long Size() const;
	virtual Stats GetStats() const;

	virtual void Print() const;

public: // modifiers
	virtual bool Add(const NonStrictMarking& marking);
	virtual const NonStrictMarking& GetNextUnexplored();

private:
	HashMap markings_storage;
	WaitingList waiting_list;
};

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* PWLIST_HPP_ */
