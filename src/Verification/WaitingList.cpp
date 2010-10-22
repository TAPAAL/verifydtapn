#include "WaitingList.hpp"
#include "Node.hpp"

namespace VerifyTAPN
{
	template<class InputIterator>
	bool AllElementsAreWatingOrCovered ( InputIterator first, InputIterator last )
	{
		for ( ;first!=last; first++)
		{
			Node* node = *first;
			if ( node->GetColor() != WAITING && node->GetColor() != PASSED )
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
		if(Size() == 0) return NULL;
		Node* node = stack.back();
		//assert(ElementsAreEqual(stack, copies));
		while(node->GetColor() == COVERED){
			Pop();
			node = stack.back();
			if(node == NULL) return NULL;
		}

		Pop();
		node->Recolor(PASSED);
		//assert(AllElementsAreWatingOrCovered(stack.begin(), stack.end()));
		return node;
	}

	long long StackWaitingList::Size() const
	{
		return actualSize;
	}
}
