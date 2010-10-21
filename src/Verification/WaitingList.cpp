#include "WaitingList.hpp"
#include "Node.hpp"

namespace VerifyTAPN
{
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
			stack.push(node);
			actualSize++;
		}
	}

	Node* StackWaitingList::Next()
	{
		if(Size() == 0) return NULL;
		Node* node = stack.top();

		while(node->GetColor() == COVERED){
			Pop();
			node = stack.top();
			if(node == NULL) return NULL;
		}

		Pop();
		node->Recolor(PASSED);
		return node;
	}

	long long StackWaitingList::Size() const
	{
		return actualSize;
	}
}
