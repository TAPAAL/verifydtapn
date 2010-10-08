#ifndef WAITINGLIST_HPP_
#define WAITINGLIST_HPP_

#include <queue>
#include <stack>

namespace VerifyTAPN{
	class Node;

	class WaitingList {
	public:
		WaitingList() { };
		virtual void Add(Node* node) = 0;
		virtual Node* Next() = 0;
		virtual long long Size() const = 0;
	};

	class QueueWaitingList : public WaitingList{
	public:
		QueueWaitingList() : queue() { };
	public:
		virtual void Add(Node* node);
		virtual Node* Next();
		virtual long long Size() const;
	private:
		std::queue<Node*> queue;
	};

	class StackWaitingList : public WaitingList{
		public:
		StackWaitingList() : stack() { };
		public:
			virtual void Add(Node* node);
			virtual Node* Next();
			virtual long long Size() const;
		private:
			std::stack<Node*> stack;
		};


}


#endif /* WAITINGLIST_HPP_ */
