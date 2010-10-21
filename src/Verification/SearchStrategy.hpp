#ifndef SEARCHSTRATEGY_HPP_
#define SEARCHSTRATEGY_HPP_

#include "WaitingList.hpp"
#include "PWList.hpp"
#include "../Core/QueryChecker.hpp"
#include "../Core/VerificationOptions.hpp"

namespace VerifyTAPN
{
	class PassedWaitingList;
	class SymMarking;

	namespace TAPN{
		class TimedArcPetriNet;
	}

	namespace AST{
		class Query;
	}


	class SearchStrategy
	{
	public:
		virtual ~SearchStrategy() { };
		virtual bool Execute() = 0;
	};



	class DFS : public SearchStrategy
	{
	public:
		DFS(
			const VerifyTAPN::TAPN::TimedArcPetriNet& tapn,
			const SymMarking& initialMarking,
			const AST::Query* query,
			const VerificationOptions& options
		);
		virtual ~DFS() { delete pwList; };
		virtual bool Execute();
		virtual bool CheckQuery(const SymMarking& marking) const;
	private:
		PassedWaitingList* pwList;
		const VerifyTAPN::TAPN::TimedArcPetriNet& tapn;
		const SymMarking& initialMarking;
		const QueryChecker checker;
		const VerificationOptions& options;
	};
}

#endif /* SEARCHSTRATEGY_HPP_ */
