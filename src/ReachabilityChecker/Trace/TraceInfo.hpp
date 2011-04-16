#ifndef TRACEINFO_HPP_
#define TRACEINFO_HPP_

#include "../../Core/TAPN/TimeInterval.hpp"
#include <vector>
#include "../../Core/SymbolicMarking/SymbolicMarking.hpp"
#include "../../Core/IndirectionTable.hpp"

namespace VerifyTAPN
{
	class Participant
	{
	public:
		Participant(int tokenIndex, const TAPN::TimeInterval& ti, int placementAfterFiring)
			: tokenIndex(tokenIndex), ti(ti), placementAfterFiring(placementAfterFiring) { };
	public:
		int TokenIndex() const { return tokenIndex; };
		int PlacementAfterFiring() const { return placementAfterFiring; };
		inline const TAPN::TimeInterval& GetTimeInterval() const { return ti; };
	private:
		int tokenIndex;
		TAPN::TimeInterval ti;
		int placementAfterFiring;
	};

	class TraceInfo
	{
	public:
		TraceInfo(id_type prevState, int transitionIndex, id_type stateId)
			: stateId(stateId), prevState(prevState), transitionIndex(transitionIndex), indirectionTable(), symmetric_trace_mapping(), transitionFiringMapping() {};
	public:
		inline id_type StateId() const { return stateId; };
		inline id_type PreviousStateId() const { return prevState; };
		inline int	TransitionIndex() const { return transitionIndex; };
		inline const std::vector<Participant>& Participants() const { return participants; };
		inline std::vector<Participant>& Participants() { return participants; };
		inline const IndirectionTable& GetSymmetricMapping() const { return indirectionTable; };
		inline IndirectionTable& GetSymmetricMapping() { return indirectionTable; };
		inline const std::vector<unsigned int>& GetOriginalMapping() const { return symmetric_trace_mapping; };
		inline const IndirectionTable& GetTransitionFiringMapping() const { return transitionFiringMapping; };
	public:
		inline void AddParticipant(const Participant& participant) { participants.push_back(participant); };
		inline void setMarking(const SymbolicMarking* marking) { this->marking = marking; };
		inline void SetSymmetricMapping(const IndirectionTable& indirectionTable) { this->indirectionTable = indirectionTable; };
		inline void SetOriginalMapping(const std::vector<unsigned int>& mapping) { symmetric_trace_mapping = mapping; };
		inline void SetTransitionFiringMapping(const IndirectionTable& mapping) { transitionFiringMapping = mapping; };
		inline const SymbolicMarking& Marking() const { return *marking; }; // TODO: get rid of
	private: // data
		id_type stateId;
		id_type prevState;
		int transitionIndex;
		std::vector<Participant> participants;
		IndirectionTable indirectionTable;
		std::vector<unsigned int> symmetric_trace_mapping;
		const SymbolicMarking* marking;
		IndirectionTable transitionFiringMapping;
	};
}

#endif /* TRACEINFO_HPP_ */
