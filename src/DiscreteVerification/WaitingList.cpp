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

void StackWaitingList::Add(const NonStrictMarking& marking)
	{
		if(marking){
			stack.push_front(marking);
		}
	}

const NonStrictMarking& StackWaitingList::Next()
	{
		if(Size() == 0) return NULL;
		NonStrictMarking& marking = stack.front();
		stack.pop_front();
		return marking;
	}

	StackWaitingList::~StackWaitingList()
	{
		stack.clear();
	}

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
