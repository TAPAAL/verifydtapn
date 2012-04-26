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
#include <stack>

namespace VerifyTAPN {
namespace DiscreteVerification {


void StackWaitingList::Add(NonStrictMarking* marking)
	{
		stack.push(marking);
	}

NonStrictMarking* StackWaitingList::Next()
	{
		assert(Size() > 0);
		NonStrictMarking* marking = stack.top();
		stack.pop();
		return marking;
	}

	StackWaitingList::~StackWaitingList()
	{
		while(!stack.empty()){
			stack.pop();
		}
	}

	std::ostream& operator<<(std::ostream& out, WaitingList& x){
		out << x.Size();
		return out;
	}


	void QueueWaitingList::Add(NonStrictMarking* marking)
		{
			queue.push(marking);
		}

	NonStrictMarking* QueueWaitingList::Next()
		{
			assert(Size() > 0);
			NonStrictMarking* marking = queue.front();
			queue.pop();
			return marking;
		}

		QueueWaitingList::~QueueWaitingList()
		{
			while(!queue.empty()){
				queue.pop();
			}
		}


} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
