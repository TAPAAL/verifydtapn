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


		void HeuristicWaitingList::Add(NonStrictMarking* marking)
		{
			WeightedMarking* weighted_marking = new WeightedMarking;
			weighted_marking->marking = marking;
			weighted_marking->weight = calculateWeight(marking);
			queue.push(weighted_marking);
		}

		NonStrictMarking* HeuristicWaitingList::Next()
		{
			assert(Size() > 0);
			WeightedMarking* weighted_marking = queue.top();
			NonStrictMarking* marking = weighted_marking->marking;
			queue.pop();
			return marking;
		}

		int HeuristicWaitingList::calculateWeight(NonStrictMarking* marking)
		{
			//TODO: Calculate weight
			return 1;
		}

		HeuristicWaitingList::~HeuristicWaitingList()
		{
			while(!queue.empty()){
				queue.pop();
			}
		}


		void RandomWaitingList::Add(NonStrictMarking* marking)
				{
					WeightedMarking* weighted_marking = new WeightedMarking;
					weighted_marking->marking = marking;
					weighted_marking->weight = calculateWeight(marking);
					queue.push(weighted_marking);
				}

				NonStrictMarking* RandomWaitingList::Next()
				{
					assert(Size() > 0);
					WeightedMarking* weighted_marking = queue.top();
					NonStrictMarking* marking = weighted_marking->marking;
					queue.pop();
					return marking;
				}

				int RandomWaitingList::calculateWeight(NonStrictMarking* marking)
				{
					return rand();
				}

				RandomWaitingList::~RandomWaitingList()
				{
					while(!queue.empty()){
						queue.pop();
					}
				}


} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
