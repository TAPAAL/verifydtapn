/*
 * WaitingList.hpp
 *
 *  Created on: 01/03/2012
 *      Author: MathiasGS
 */

#ifndef WAITINGLIST_HPP_
#define WAITINGLIST_HPP_

#include "NonStrictMarking.hpp"
#include <deque>
#include <stack>
#include "boost/optional.hpp"
#include "boost/shared_ptr.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

class WaitingList {
public:
	WaitingList() {};
	virtual ~WaitingList() {};
	virtual void Add(NonStrictMarking* marking) = 0;
	virtual NonStrictMarking* Next() = 0;
	virtual size_t Size() = 0;
	friend std::ostream& operator<<(std::ostream& out, WaitingList& x);
};

class StackWaitingList : public WaitingList{
public:
	StackWaitingList() : stack() { };
	virtual ~StackWaitingList();
public:
	virtual void Add(NonStrictMarking* marking);
	virtual NonStrictMarking* Next();
	virtual size_t Size() { return stack.size(); };
private:
	std::stack< NonStrictMarking* > stack;
};

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* WAITINGLIST_HPP_ */
