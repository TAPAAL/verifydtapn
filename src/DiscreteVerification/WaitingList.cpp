/*
 * WaitingList.cpp
 *
 *  Created on: 01/03/2012
 *      Author: MathiasGS
 */

#include "../Core/QueryParser/NormalizationVisitor.hpp"
#include "WaitingList.hpp"
#include "assert.h"
#include <queue>
#include <deque>
#include <stack>
#include <vector>

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

void HeuristicStackWaitingList::Add(NonStrictMarking* marking)
{
	WeightedMarking* weighted_marking = new WeightedMarking;
	weighted_marking->marking = marking;
	weighted_marking->weight = calculateWeight(marking);
	buffer.push(weighted_marking);
}

void HeuristicStackWaitingList::flushBuffer(){
	while(!buffer.empty()){
		stack.push(buffer.top()->marking);
		buffer.pop();
	}
}

NonStrictMarking* HeuristicStackWaitingList::Next()
{
	assert(Size() > 0);
	flushBuffer();
	NonStrictMarking* marking = stack.top();
	stack.pop();
	return marking;
}

int HeuristicStackWaitingList::calculateWeight(NonStrictMarking* marking)
{
	LivenessWeightQueryVisitor weight(*marking);
	boost::any weight_c;
	query->Accept(weight, weight_c);
	return boost::any_cast<int>(weight_c);
}

AST::Query* HeuristicStackWaitingList::normalizeQuery(AST::Query* q){
	AST::NormalizationVisitor visitor;
	return visitor.Normalize(*q);
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
	WeightQueryVisitor weight(*marking);
	boost::any weight_c;
	query->Accept(weight, weight_c);
	return boost::any_cast<int>(weight_c);
}

AST::Query* HeuristicWaitingList::normalizeQuery(AST::Query* q){
	AST::NormalizationVisitor visitor;
	return visitor.Normalize(*q);
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

void RandomStackWaitingList::Add(NonStrictMarking* marking)
{
	WeightedMarking* weighted_marking = new WeightedMarking;
	weighted_marking->marking = marking;
	weighted_marking->weight = calculateWeight(marking);
	buffer.push(weighted_marking);
}

void RandomStackWaitingList::flushBuffer(){
	while(!buffer.empty()){
		stack.push(buffer.top()->marking);
		buffer.pop();
	}
}

NonStrictMarking* RandomStackWaitingList::Next()
{
	assert(Size() > 0);
	flushBuffer();
	NonStrictMarking* marking = stack.top();
	stack.pop();
	return marking;
}

int RandomStackWaitingList::calculateWeight(NonStrictMarking* marking)
{
	return rand();
}

RandomStackWaitingList::~RandomStackWaitingList()
{
	while(!stack.empty()){
		stack.pop();
	}
}


} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
