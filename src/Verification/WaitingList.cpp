#include "WaitingList.hpp"
#include "Node.hpp"
#include "assert.h"

namespace VerifyTAPN
{
	template<class InputIterator>
	bool AllElementsAreWatingOrCovered ( InputIterator first, InputIterator last )
	{
		for ( ;first!=last; first++)
		{
			Node* node = *first;
			if ( node->GetColor() != WAITING && node->GetColor() != COVERED )
			{
				return false;
			}
		}
		return true;
	}


	void QueueWaitingList::Add(Node* node)
	{
		if(node){
			queue.push(node);
			actualSize++;
		}
	}

	Node* QueueWaitingList::Next()
	{
		if(Size() == 0) return NULL;
		Node* node = queue.front();

		while(node->GetColor() == COVERED){
			Pop();
			node = queue.front();
			if(node == NULL) return NULL;
		}

		Pop();
		node->Recolor(PASSED);
		return node;
	}

	long long QueueWaitingList::Size() const
	{
		return actualSize;
	}

	void StackWaitingList::Add(Node* node)
	{
		if(node){
			assert(node->GetColor()==WAITING);
			stack.push_back(node);
			copies.push_back(*node);
			actualSize++;
		}
	}

	Node* StackWaitingList::Next()
	{
		long long size = Size();
		int decBy = 0;
		if(size == 0) return NULL;
		Node* node = stack.back();
		assert(node->GetColor() == WAITING || node->GetColor() == COVERED);
		while(node->GetColor() == COVERED){
			Pop();decBy++;
			node = stack.back();
			if(node == NULL) return NULL;
		}

		Pop();decBy++;
		node->Recolor(PASSED);
		assert(Size() == size-decBy);
		assert(AllElementsAreWatingOrCovered(stack.begin(), stack.end()));
		return node;
	}

	long long StackWaitingList::Size() const
	{
		return actualSize;
	}
}
