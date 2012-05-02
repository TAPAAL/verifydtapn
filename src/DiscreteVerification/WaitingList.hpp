/*
 * WaitingList.hpp
 *
 *  Created on: 01/03/2012
 *      Author: MathiasGS
 */

#ifndef WAITINGLIST_HPP_
#define WAITINGLIST_HPP_

#include "NonStrictMarking.hpp"
#include <queue>
#include <stack>
#include "boost/optional.hpp"
#include "boost/shared_ptr.hpp"
#include "../Core/QueryParser/AST.hpp"
#include "WeightQueryVisitor.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

struct Stats{
		long long exploredStates;
		long long discoveredStates;
		long long storedStates;

		Stats() : exploredStates(0), discoveredStates(0), storedStates(0) {};
};

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

class QueueWaitingList : public WaitingList{
public:
	QueueWaitingList() : queue() { };
	virtual ~QueueWaitingList();
public:
	virtual void Add(NonStrictMarking* marking);
	virtual NonStrictMarking* Next();
	virtual size_t Size() { return queue.size(); };
private:
	std::queue< NonStrictMarking* > queue;
};

struct WeightedMarking{
	NonStrictMarking* marking;
	int weight;
};

struct less : public std::binary_function<WeightedMarking*, WeightedMarking*, bool>
{
	bool operator()(const WeightedMarking* x, const WeightedMarking* y) const
	{
		return x->weight < y->weight;
	}
};

class HeuristicWaitingList : public WaitingList{
public:
		typedef std::priority_queue<WeightedMarking*, std::vector<WeightedMarking*>, less > priority_queue;
public:
	HeuristicWaitingList(AST::Query* q) : queue(), query(normalizeQuery(q)) { };
	virtual ~HeuristicWaitingList();
public:
	virtual void Add(NonStrictMarking* marking);
	virtual NonStrictMarking* Next();
	virtual size_t Size() { return queue.size(); };
private:
	int calculateWeight(NonStrictMarking* marking);
	AST::Query* normalizeQuery(AST::Query* q);
	priority_queue queue;
	AST::Query* query;
};

class RandomWaitingList : public WaitingList{
public:
		typedef std::priority_queue<WeightedMarking*, std::vector<WeightedMarking*>, less > priority_queue;
public:
	RandomWaitingList() : queue() { };
	virtual ~RandomWaitingList();
public:
	virtual void Add(NonStrictMarking* marking);
	virtual NonStrictMarking* Next();
	virtual size_t Size() { return queue.size(); };
private:
	int calculateWeight(NonStrictMarking* marking);
	priority_queue queue;
};

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* WAITINGLIST_HPP_ */
