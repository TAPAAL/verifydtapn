#ifndef DFS_HPP_
#define DFS_HPP_

#include "SearchStrategy.hpp"

namespace VerifyTAPN
{
	class DFS : public DefaultSearchStrategy
	{
	public:
		DFS(
			const VerifyTAPN::TAPN::TimedArcPetriNet& tapn,
			SymbolicMarking* initialMarking,
			const AST::Query* query,
			const VerificationOptions& options,
			MarkingFactory* factory
		) : DefaultSearchStrategy(tapn, initialMarking, query, options, factory) { };

	protected:
		virtual WaitingList* CreateWaitingList() const { return new StackWaitingList; };
	};

}

#endif /* DFS_HPP_ */
