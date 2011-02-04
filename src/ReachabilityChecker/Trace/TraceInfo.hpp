#ifndef TRACEINFO_HPP_
#define TRACEINFO_HPP_

#include "../../Core/TAPN/TimeInterval.hpp"
#include <vector>
#include "../../Core/SymbolicMarking/SymMarking.hpp"

namespace VerifyTAPN
{
	class Participant
	{
	public:
		Participant(int tokenIndex, const TAPN::TimeInterval& ti, int indexAfterFiring)
			: tokenIndex(tokenIndex), ti(ti), indexAfterFiring(indexAfterFiring) { };
	public:
		inline int TokenIndex() const { return tokenIndex; };
		inline const TAPN::TimeInterval& GetTimeInterval() const { return ti; };
		inline int IndexAfterFiring() const { return indexAfterFiring; };
	private:
		int tokenIndex;
		TAPN::TimeInterval ti;
		int indexAfterFiring;
	};

	class TraceInfo
	{
	public:
		typedef long long id_type;
	public:
		TraceInfo(id_type prevState, int transitionIndex, id_type stateId)
			: stateId(stateId), prevState(prevState), transitionIndex(transitionIndex) {};
	public:
		inline id_type StateId() const { return stateId; };
		inline id_type PreviousStateId() const { return prevState; };
		inline int	TransitionIndex() const { return transitionIndex; };
		inline const std::vector<Participant>& Participants() const { return participants; };
	public:
		inline void AddParticipant(const Participant& participant) { participants.push_back(participant); };
		inline void setMarking(const SymMarking* marking) { this->marking = marking; };
		inline const SymMarking& Marking() const { return *marking; };
	private: // data
		id_type stateId;
		id_type prevState;
		int transitionIndex;
		std::vector<Participant> participants;
		const SymMarking* marking;
	};
}

#endif /* TRACEINFO_HPP_ */
