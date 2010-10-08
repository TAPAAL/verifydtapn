#include "WaitingList.hpp"
#include "Node.hpp"

namespace VerifyTAPN
{
	void QueueWaitingList::Add(Node* node)
	{
		if(node){
			queue.push(node);
		}
	}

	Node* QueueWaitingList::Next()
	{
		if(Size() == 0) return NULL;

		Node* node = queue.front();
		queue.pop();
		node->Recolor(PASSED);
		return node;
	}

	long long QueueWaitingList::Size() const
	{
		return queue.size();
	}

	void StackWaitingList::Add(Node* node)
	{
		if(node){
			stack.push(node);
		}
	}

	Node* StackWaitingList::Next()
	{
		if(Size() == 0) return NULL;

		Node* node = stack.top();
		stack.pop();
		node->Recolor(PASSED);
		return node;
	}

	long long StackWaitingList::Size() const
	{
		return stack.size();
	}
}
