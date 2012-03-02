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

namespace VerifyTAPN {
namespace DiscreteVerification {

class WaitingList {
public:
	WaitingList() {};
	virtual ~WaitingList() {};
	virtual void Add(const NonStrictMarking& marking) = 0;
	virtual const NonStrictMarking* Next() = 0;
	virtual long long Size() const = 0;
};

class StackWaitingList : public WaitingList{
public:
	StackWaitingList() : stack() { };
	virtual ~StackWaitingList();
public:
	virtual void Add(const NonStrictMarking& marking);
	virtual const NonStrictMarking* Next();
	virtual long long Size() const { return stack.size(); };
private:
	std::deque<const NonStrictMarking*> stack;
};

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* WAITINGLIST_HPP_ */
