#ifndef SEARCHSTRATEGY_HPP_
#define SEARCHSTRATEGY_HPP_

#include "WaitingList.hpp"
#include "PWList.hpp"

namespace VerifyTAPN
{
	class PassedWaitingList;
	class SymMarking;

	namespace TAPN{
		class TimedArcPetriNet;
	}


	class SearchStrategy
	{
	public:
		virtual ~SearchStrategy() { };
		virtual bool Execute() = 0;
	};



	class DFS : SearchStrategy
	{
	public:
		DFS(const VerifyTAPN::TAPN::TimedArcPetriNet& tapn,
				const SymMarking& initialMarking)
			: tapn(tapn), initialMarking(initialMarking)
			  { pwList = new PWList(new StackWaitingList); };
		virtual ~DFS() { delete pwList; };
		virtual bool Execute();
	private:
		PassedWaitingList* pwList;
		const VerifyTAPN::TAPN::TimedArcPetriNet& tapn;
		const SymMarking& initialMarking;
	};
}

#endif /* SEARCHSTRATEGY_HPP_ */
