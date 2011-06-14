#ifndef PRIORITYQUEUEWAITINGLIST_HPP_
#define PRIORITYQUEUEWAITINGLIST_HPP_

#include "WaitingList.hpp"
#include <queue>
#include "Node.hpp"

namespace VerifyTAPN {

	struct WeightedNode{
		Node* node;
		int weight;
		virtual ~WeightedNode() { if(node->GetColor() == COVERED) delete node; };
	};

	struct less : public std::binary_function<WeightedNode*, WeightedNode*, bool>
	{
		bool operator()(const WeightedNode* x, const WeightedNode* y) const
		{
			return x->weight < y->weight;
		}
	};

	template <typename CalcWeight>
	class PriorityQueueWaitingList : public VerifyTAPN::WaitingList {
	public:
		typedef std::priority_queue<WeightedNode*, std::vector<WeightedNode*>, less > priority_queue;
	public:
		PriorityQueueWaitingList(): calcWeight(), queue(), actualSize(0) { };
		virtual ~PriorityQueueWaitingList()
		{
			while(!queue.empty())
			{
				WeightedNode* it = queue.top();
				delete it;
				queue.pop();
			}
		}
	public:
		virtual void Add(Node* node);
		virtual Node* Next();
		virtual long long Size() const { return actualSize; };
		inline virtual long long SizeIncludingCovered() const { return queue.size(); };
		inline virtual void DecrementActualSize() { actualSize--; };

	private:
		CalcWeight calcWeight;
		priority_queue queue;
		long long actualSize;
	};

	template <typename CalcWeight>
		void PriorityQueueWaitingList<CalcWeight>::Add(Node* node)
		{
			if(node){
				assert(node->GetColor()==WAITING);
				WeightedNode* wnode = new WeightedNode;
				wnode->node = node;
				wnode->weight = calcWeight(node);
				queue.push(wnode);
				actualSize++;
			}
		}

		template <typename CalcWeight>
		Node* PriorityQueueWaitingList<CalcWeight>::Next()
		{
			if(Size() == 0) return NULL;
			WeightedNode* node = queue.top();
			assert(node->node->GetColor() == WAITING || node->node->GetColor() == COVERED);
			while(node->node->GetColor() == COVERED){
				queue.pop();
				delete node;
				node = queue.top();
				if(node == NULL || node->node == NULL) return NULL;
			}

			queue.pop(); actualSize--;
			node->node->Recolor(PASSED);
	//		assert(AllElementsAreWatingOrCovered(stack.begin(), stack.end()));
			return node->node;
		}
}

#endif /* PRIORITYQUEUEWAITINGLIST_HPP_ */
