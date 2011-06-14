#ifndef RANDOMSEARCH_HPP_
#define RANDOMSEARCH_HPP_

#include "SearchStrategy.hpp"
#include "../PassedWaitingList/PriorityQueueWaitingList.hpp"
#include "../PassedWaitingList/Node.hpp"

namespace VerifyTAPN
{
	struct Random : public std::unary_function<Node*, int>
	{
		int operator()(const Node* node) const
		{
			return rand() % 1000;
		}
	};

	class RandomSearch : public DefaultSearchStrategy
	{
	public:
		RandomSearch(
			const VerifyTAPN::TAPN::TimedArcPetriNet& tapn,
			SymbolicMarking* initialMarking,
			const AST::Query* query,
			const VerificationOptions& options,
			MarkingFactory* factory
		) : DefaultSearchStrategy(tapn, initialMarking, query, options, factory) { };

	protected:
		virtual WaitingList* CreateWaitingList() const { return new PriorityQueueWaitingList<Random>; };
	};

}

#endif /* RANDOMSEARCH_HPP_ */
