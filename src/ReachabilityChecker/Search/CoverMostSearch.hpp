#ifndef COVERMOSTSEARCH_HPP_
#define COVERMOSTSEARCH_HPP_

#include "SearchStrategy.hpp"
#include "../PassedWaitingList/PriorityQueueWaitingList.hpp"
#include "../PassedWaitingList/Node.hpp"
#include "../../Core/SymbolicMarking/DiscretePartInclusionMarking.hpp"

namespace VerifyTAPN
{
	struct CalcWeight : public std::unary_function<Node*, int>
	{
		int operator()(const Node* node) const
		{
			int weight = 0;
			const DiscretePartInclusionMarking& other = static_cast<const DiscretePartInclusionMarking&>(*node->GetMarking());
			const std::vector<int>& inc = other.inclusionTokens();
			for(std::vector<int>::const_iterator it = inc.begin(); it != inc.end(); it++)
			{
				weight += *it;
			}
			return weight;
		}
	};

	class CoverMostSearch : public DefaultSearchStrategy
	{
	public:
		CoverMostSearch(
			const VerifyTAPN::TAPN::TimedArcPetriNet& tapn,
			SymbolicMarking* initialMarking,
			const AST::Query* query,
			const VerificationOptions& options,
			MarkingFactory* factory
		) : DefaultSearchStrategy(tapn, initialMarking, query, options, factory) { };

	protected:
		virtual WaitingList* CreateWaitingList() const { return new PriorityQueueWaitingList<CalcWeight>; };
	};

}

#endif /* COVERMOSTSEARCH_HPP_ */
