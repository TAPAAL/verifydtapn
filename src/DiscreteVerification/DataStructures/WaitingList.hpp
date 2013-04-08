/*
 * WaitingList.hpp
 *
 *  Created on: 01/03/2012
 *      Author: MathiasGS
 */

#ifndef WAITINGLIST_HPP_
#define WAITINGLIST_HPP_

#include "NonStrictMarking.hpp"
#include "TimeDart.hpp"
#include <queue>
#include <stack>
#include <vector>
#include "boost/optional.hpp"
#include "boost/shared_ptr.hpp"
#include "../../Core/QueryParser/AST.hpp"
#include "../SearchStrategies/WeightQueryVisitor.hpp"
#include "../SearchStrategies/LivenessWeightQueryVisitor.hpp"
#include "../../Core/QueryParser/NormalizationVisitor.hpp"
#include "assert.h"
#include <queue>
#include <deque>
#include <stack>
#include <vector>
#include "PData.h"

namespace VerifyTAPN {
namespace DiscreteVerification {

template <class T>
class WaitingList {
public:
	WaitingList() {};
	virtual ~WaitingList() {};
	virtual void Add(NonStrictMarkingBase* weight, T* payload) = 0;
	virtual T* Peek() = 0;
	virtual T* Pop() = 0;
	virtual size_t Size() = 0;
	virtual void flushBuffer(){};
	template <class S>
	friend std::ostream& operator<<(std::ostream& out, WaitingList<S>& x);
};

template <class T>
class StackWaitingList : public WaitingList<T>{
public:
	StackWaitingList() : stack() { };
	virtual ~StackWaitingList();
public:
	virtual void Add(NonStrictMarkingBase* weight, T* payload);
        virtual T* Peek();
	virtual T* Pop();
	virtual size_t Size() { return stack.size(); };
	virtual void flushBuffer(){};
protected:
	std::stack<T*> stack;
};

template <class T>
struct WeightedItem{
	T* item;
	int weight;
};

template <class T>
struct less : public std::binary_function<WeightedItem<T>*, WeightedItem<T>*, bool>
{
	bool operator()(const WeightedItem<T>* x, const WeightedItem<T>* y) const
	{
		return x->weight > y->weight;
	}
};

template <class T>
class HeuristicStackWaitingList : public StackWaitingList<T>{
public:
	typedef std::priority_queue<WeightedItem<T>*, std::vector<WeightedItem<T> * >, less<T> > priority_queue;
	HeuristicStackWaitingList(AST::Query* q) : buffer(), query(normalizeQuery(q)) { };
	virtual void Add(NonStrictMarkingBase* weight, T* payload);
	virtual T* Peek();
	virtual T* Pop();
	virtual size_t Size() { return this->stack.size() +buffer.size(); };
	virtual void flushBuffer();
private:
	int calculateWeight(NonStrictMarkingBase* marking);
	int calculateWeight(LivenessDart* marking);
	int calculateWeight(WaitingDart* marking);
	AST::Query* normalizeQuery(AST::Query* q);
	priority_queue buffer;
	AST::Query* query;
};

template <class T>
class QueueWaitingList : public WaitingList<T>{
public:
	QueueWaitingList() : queue() { };
	virtual ~QueueWaitingList();
public:
	virtual void Add(NonStrictMarkingBase* weight, T* payload);
	virtual T* Peek();
	virtual T* Pop();
	virtual size_t Size() { return queue.size(); };
private:
	std::queue< T* > queue;
};

template <class T>
class HeuristicWaitingList : public WaitingList<T>{
public:
	typedef std::priority_queue<WeightedItem<T>*, std::vector<WeightedItem<T>* >, less<T> > priority_queue;
public:
	HeuristicWaitingList(AST::Query* q) : queue(), query(normalizeQuery(q)) { };
	virtual ~HeuristicWaitingList();
public:
	virtual void Add(NonStrictMarkingBase* weight, T* payload);
	virtual T* Peek();
	virtual T* Pop();
	virtual size_t Size() { return queue.size(); };
private:
	AST::Query* normalizeQuery(AST::Query* q);
	int calculateWeight(NonStrictMarkingBase* marking);
	int calculateWeight(LivenessDart* marking);
	priority_queue queue;
	AST::Query* query;
};

template <class T>
class RandomStackWaitingList : public StackWaitingList<T>{
public:
	typedef std::priority_queue<WeightedItem<T>*, std::vector<WeightedItem<T>*>, less<T> > priority_queue;
public:
	RandomStackWaitingList() : buffer() { };
	virtual ~RandomStackWaitingList();
public:
	virtual void Add(NonStrictMarkingBase* weight, T* payload);
	virtual T* Peek();
	virtual T* Pop();
	virtual size_t Size() { return this->stack.size()+buffer.size(); };
	virtual void flushBuffer();
private:
	int calculateWeight(NonStrictMarkingBase* marking);
	priority_queue buffer;
};

template <class T>
class RandomWaitingList : public WaitingList<T>{
public:
	typedef std::priority_queue<WeightedItem<T>*, std::vector<WeightedItem<T>*>, less<T> > priority_queue;
public:
	RandomWaitingList() : queue() { };
	virtual ~RandomWaitingList();
public:
	virtual void Add(NonStrictMarkingBase* weight, T* payload);
	virtual T* Peek();
	virtual T* Pop();
	virtual size_t Size() { return queue.size(); };
private:
	int calculateWeight(NonStrictMarkingBase* marking);
	priority_queue queue;
};


//IMPLEMENTATIONS
template <class T>
void StackWaitingList<T>::Add(NonStrictMarkingBase* weight, T* payload)
{
	stack.push(payload);
}

template <class T>
T* StackWaitingList<T>::Pop()
{
	T* marking = stack.top();
	stack.pop();
	return marking;
}

template <class T>
T* StackWaitingList<T>::Peek()
{
	T* marking = stack.top();
	return marking;
}

template <class T>
StackWaitingList<T>::~StackWaitingList()
{
	while(!stack.empty()){
		stack.pop();
	}
}

template <class T>
std::ostream& operator<<(std::ostream& out, WaitingList<T>& x){
	out << x.Size();
	return out;
}

template <class T>
void HeuristicStackWaitingList<T>::Add(NonStrictMarkingBase* weight, T* payload)
{
	WeightedItem<T>* weighted_item = new WeightedItem<T>;
	weighted_item->item = payload;
	weighted_item->weight = calculateWeight(weight);
	buffer.push(weighted_item);
}

template <class T>
void HeuristicStackWaitingList<T>::flushBuffer(){
	while(!buffer.empty()){
		this->stack.push(buffer.top()->item);
                delete buffer.top();
		buffer.pop();
	}
}

template <class T>
T* HeuristicStackWaitingList<T>::Pop()
{
	flushBuffer();
	T* marking = this->stack.top();
	this->stack.pop();
	return marking;
}

template <class T>
T* HeuristicStackWaitingList<T>::Peek()
{
	flushBuffer();
	T* marking = this->stack.top();
	return marking;
}

template <class T>
int HeuristicStackWaitingList<T>::calculateWeight(NonStrictMarkingBase* marking)
{
	LivenessWeightQueryVisitor weight(*marking);
	boost::any weight_c;
	query->Accept(weight, weight_c);
	return boost::any_cast<int>(weight_c);
}

template <class T>
int HeuristicStackWaitingList<T>::calculateWeight(LivenessDart* dart)
{
	return calculateWeight(dart->getBase());
}

template <class T>
int HeuristicStackWaitingList<T>::calculateWeight(WaitingDart* dart)
{
	return calculateWeight(dart->dart->getBase());
}


template <class T>
AST::Query* HeuristicStackWaitingList<T>::normalizeQuery(AST::Query* q){
	AST::NormalizationVisitor visitor;
	return visitor.Normalize(*q);
}

template <class T>
void QueueWaitingList<T>::Add(NonStrictMarkingBase* weight, T* payload)
{
	queue.push(payload);
}

template <class T>
T* QueueWaitingList<T>::Pop()
{
	T* marking = queue.front();
	queue.pop();
	return marking;
}

template <class T>
T* QueueWaitingList<T>::Peek()
{
	T* marking = queue.front();
	return marking;
}

template <class T>
QueueWaitingList<T>::~QueueWaitingList()
{
	while(!queue.empty()){
		queue.pop();
	}
}

template <class T>
void HeuristicWaitingList<T>::Add(NonStrictMarkingBase* weight, T* payload)
{
	WeightedItem<T>* weighted_item = new WeightedItem<T>;
	weighted_item->item = payload;
	weighted_item->weight = calculateWeight(weight);
	queue.push(weighted_item);
}

template <class T>
T* HeuristicWaitingList<T>::Pop()
{
	WeightedItem<T>* weighted_item = queue.top();
	T* marking = weighted_item->item;
        delete queue.top();
	queue.pop();
	return marking;
}

template <class T>
T* HeuristicWaitingList<T>::Peek()
{
	WeightedItem<T>* weighted_item = queue.top();
	T* marking = weighted_item->item;
	return marking;
}

template <class T>
int HeuristicWaitingList<T>::calculateWeight(NonStrictMarkingBase* marking)
{
	WeightQueryVisitor weight(*marking);
	boost::any weight_c;
	query->Accept(weight, weight_c);
	return boost::any_cast<int>(weight_c);
}

template <class T>
int HeuristicWaitingList<T>::calculateWeight(LivenessDart* dart)
{
	return calculateWeight(dart->getBase());
}

template <class T>
AST::Query* HeuristicWaitingList<T>::normalizeQuery(AST::Query* q){
	AST::NormalizationVisitor visitor;
	return visitor.Normalize(*q);
}

template <class T>
HeuristicWaitingList<T>::~HeuristicWaitingList()
{
	while(!queue.empty()){
                delete queue.top();
		queue.pop();
	}
}

template <class T>
void RandomWaitingList<T>::Add(NonStrictMarkingBase* weight, T* payload)
{
	WeightedItem<T>* weighted_item = new WeightedItem<T>;
	weighted_item->marking = payload;
	weighted_item->weight = calculateWeight(weight);
	queue.push(weighted_item);
}

template <class T>
T* RandomWaitingList<T>::Pop()
{
	WeightedItem<T>* weighted_item = queue.top();
	T* marking = weighted_item->marking;
        delete queue.top();
	queue.pop();
	return marking;
}

template <class T>
T* RandomWaitingList<T>::Peek()
{
	WeightedItem<T>* weighted_item = queue.top();
	T* marking = weighted_item->marking;
	return marking;
}

template <class T>
int RandomWaitingList<T>::calculateWeight(NonStrictMarkingBase* marking)
{
	return rand();
}

template <class T>
RandomWaitingList<T>::~RandomWaitingList()
{
	while(!queue.empty()){
                delete queue.top();
		queue.pop();
	}
}

template <class T>
void RandomStackWaitingList<T>::Add(NonStrictMarkingBase* weight, T* payload)
{
	WeightedItem<T>* weighted_item = new WeightedItem<T>;
	weighted_item->item = payload;
	weighted_item->weight = calculateWeight(weight);
	buffer.push(weighted_item);
}

template <class T>
void RandomStackWaitingList<T>::flushBuffer(){
	while(!buffer.empty()){
		this->stack.push(buffer.top()->item);
                delete buffer.top();
		buffer.pop();
	}
}

template <class T>
T* RandomStackWaitingList<T>::Pop()
{
	flushBuffer();
	T* marking = this->stack.top();
	this->stack.pop();
	return marking;
}

template <class T>
T* RandomStackWaitingList<T>::Peek()
{
	flushBuffer();
	T* marking = this->stack.top();
	return marking;
}

template <class T>
int RandomStackWaitingList<T>::calculateWeight(NonStrictMarkingBase* marking)
{
	return rand();
}

template <class T>
RandomStackWaitingList<T>::~RandomStackWaitingList()
{
        flushBuffer();
	while(!this->stack.empty()){
		this->stack.pop();
	}
}

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* WAITINGLIST_HPP_ */
