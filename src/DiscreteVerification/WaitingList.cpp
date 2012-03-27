/*
 * WaitingList.cpp
 *
 *  Created on: 01/03/2012
 *      Author: MathiasGS
 */

#include "WaitingList.hpp"
#include "assert.h"
#include <queue>
#include <deque>

namespace VerifyTAPN {
namespace DiscreteVerification {


void StackWaitingList::Add(NonStrictMarking* marking)
	{
		stack.push_front(marking);
	}

NonStrictMarking* StackWaitingList::Next()
	{
		assert(Size() > 0);
		NonStrictMarking* marking = stack.back();
		stack.pop_back();
		return marking;
	}

	StackWaitingList::~StackWaitingList()
	{
		stack.clear();
	}

	std::ostream& operator<<(std::ostream& out, WaitingList& x){
		out << x.Size();
		return out;
	}


} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
