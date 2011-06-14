#ifndef BFS_HPP_
#define BFS_HPP_

#include "SearchStrategy.hpp"

namespace VerifyTAPN
{
	class BFS : public DefaultSearchStrategy
	{
	public:
		BFS(
			const VerifyTAPN::TAPN::TimedArcPetriNet& tapn,
			SymbolicMarking* initialMarking,
			const AST::Query* query,
			const VerificationOptions& options,
			MarkingFactory* factory
		) : DefaultSearchStrategy(tapn, initialMarking, query, options, factory) { };

	protected:
		virtual WaitingList* CreateWaitingList() const { return new QueueWaitingList; };
	};

}

#endif /* BFS_HPP_ */
