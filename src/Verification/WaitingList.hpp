#ifndef WAITINGLIST_HPP_
#define WAITINGLIST_HPP_

#include <queue>
#include <deque>

namespace VerifyTAPN{
	class Node;

	class WaitingList {
	public:
		WaitingList() { };
		virtual ~WaitingList() { };
		virtual void Add(Node* node) = 0;
		virtual Node* Next() = 0;
		virtual long long Size() const = 0;
		virtual long long SizeIncludingCovered() const = 0;
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
		inline virtual long long SizeIncludingCovered() const { return queue.size(); };
	private:
		std::queue<Node*> queue;
		long long actualSize;
	};

	class StackWaitingList : public WaitingList{
		public:
		StackWaitingList() : stack(), actualSize(0) { };
		virtual ~StackWaitingList();
		public:
			virtual void Add(Node* node);
			virtual Node* Next();
			virtual long long Size() const;
			inline virtual long long SizeIncludingCovered() const { return stack.size(); };
			inline virtual void DecrementActualSize() { actualSize--; };
		private:
			std::deque<Node*> stack;
			long long actualSize;
		};


}


#endif /* WAITINGLIST_HPP_ */
