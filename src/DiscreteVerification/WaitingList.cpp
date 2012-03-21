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
		std::cout << "In: " << *marking << std::endl;
		stack.push_front(marking);
	}

NonStrictMarking* StackWaitingList::Next()
	{
		assert(Size() > 0);
		NonStrictMarking* marking = stack.front();
		std::cout << "Out: " << *marking << std::endl;
		stack.pop_front();
		std::cout << "Out2: " << *marking << std::endl;
		return marking;
	}

	StackWaitingList::~StackWaitingList()
	{
		stack.clear();
	}

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
