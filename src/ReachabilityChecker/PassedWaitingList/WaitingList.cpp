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

	template<class InputIterator>
		bool AllElementsAreCovered ( InputIterator first, InputIterator last )
		{
			for ( ;first!=last; first++)
			{
				Node* node = *first;
				if ( node->GetColor() != COVERED )
				{
					return false;
				}
			}
			return true;
		}


	void QueueWaitingList::Add(Node* node)
	{
		if(node){
			queue.push_back(node);
			actualSize++;
		}
	}

	Node* QueueWaitingList::Next()
	{
		if(Size() == 0) return NULL;
		Node* node = queue.front();
		assert(node->GetColor() == WAITING || node->GetColor() == COVERED);
		while(node->GetColor() == COVERED){
			queue.pop_front();
			delete node;
			node = queue.front();
			if(node == NULL) return NULL;
		}

		queue.pop_front(); actualSize--;
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
			actualSize++;
		}
	}

	Node* StackWaitingList::Next()
	{
		if(Size() == 0) return NULL;
		Node* node = stack.back();
		assert(node->GetColor() == WAITING || node->GetColor() == COVERED);
		while(node->GetColor() == COVERED){
			stack.pop_back();
			delete node;
			node = stack.back();
			if(node == NULL) return NULL;
		}

		stack.pop_back(); actualSize--;
		node->Recolor(PASSED);
//		assert(AllElementsAreWatingOrCovered(stack.begin(), stack.end()));
		return node;
	}

	long long StackWaitingList::Size() const
	{
		return actualSize;
	}

	QueueWaitingList::~QueueWaitingList()
	{
		for(std::deque<Node*>::iterator it = queue.begin(); it != queue.end(); it++)
		{
			if((*it)->GetColor() == COVERED){
				delete *it;
			}
		}
	}

	StackWaitingList::~StackWaitingList()
	{
		for(std::deque<Node*>::iterator it = stack.begin(); it != stack.end(); it++)
		{
			if((*it)->GetColor() == COVERED){
				delete *it;
			}
		}
	}
}
