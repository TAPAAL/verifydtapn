/*
 * WaitingList.hpp
 *
 *  Created on: 01/03/2012
 *      Author: MathiasGS
 */

#ifndef WAITINGLIST_HPP_
#define WAITINGLIST_HPP_

namespace VerifyTAPN {
namespace DiscreteVerification {

class WaitingList {
public:
	WaitingList() { };
	virtual ~WaitingList() { };
	virtual void Add(const NonStrictMarking& node);
	virtual NonStrictMarking& Next();
	virtual long long Size() const;
};

class StackWaitingList : public WaitingList{
public:
	StackWaitingList() : stack() { };
	virtual ~StackWaitingList();
public:
	virtual void Add(const NonStrictMarking& marking);
	virtual const NonStrictMarking& Next();
	virtual long long Size() const { stack.size(); };
private:
	std::deque<const NonStrictMarking&> stack;
};

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* WAITINGLIST_HPP_ */
