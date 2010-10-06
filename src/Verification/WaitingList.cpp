#include "WaitingList.hpp"

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
		return node;
	}

	long long QueueWaitingList::Size() const
	{
		return queue.size();
	}
}
