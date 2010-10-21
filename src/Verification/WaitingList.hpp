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
		virtual void DecrementActualSize() = 0;
	};

	class QueueWaitingList : public WaitingList{
	public:
		QueueWaitingList() : queue(), actualSize(0) { };
	public:
		virtual void Add(Node* node);
		virtual Node* Next();
		virtual long long Size() const;
	public:
		inline virtual void DecrementActualSize() { actualSize--; };
	private:
		inline void Pop() { queue.pop(); actualSize--; };
	private:
		std::queue<Node*> queue;
		long long actualSize;
	};

	class StackWaitingList : public WaitingList{
		public:
		StackWaitingList() : stack(), actualSize(0) { };
		public:
			virtual void Add(Node* node);
			virtual Node* Next();
			virtual long long Size() const;
			inline virtual void DecrementActualSize() { actualSize--; };
		private:
			inline void Pop() { stack.pop(); actualSize--; };
		private:
			std::stack<Node*> stack;
			long long actualSize;
		};


}


#endif /* WAITINGLIST_HPP_ */