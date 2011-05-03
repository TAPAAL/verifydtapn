#ifndef TRACEINFO_HPP_
#define TRACEINFO_HPP_

#include <vector>
#include "../../Core/SymbolicMarking/SymbolicMarking.hpp"
#include "../../Core/IndirectionTable.hpp"

/*
 * This class contains metadata associated with a transition firing.
 * It is used to generate concrete traces based on the symbolic exploration.
 *
 * Assume that M is the current marking. When we calculate a successor, it looks like
 * the following:
 *
 * M --t--> M' --symmetry--> M''
 *
 * To be able to generate traces for symmetric markings we employ two bijections:
 * TransitionFiringMapping: Maps token indices in M to token indices in M'.
 * 							This particular bijection is by default of size k, meaning
 * 							it maps all k tokens across the transition firing including
 * 							those in bottom which are not explicitly represented in M.
 * SymmetricMapping:		Maps tokens indices in M' to token indices in M''.
 * 							This particular bijection is NOT NECESSARILY of size k,
 * 							which is why TraceStore augments such that it maps all k
 * 							tokens, before using it.
 *
 * Based on the two bijections (i.e. they map both forward and backwards) TraceStore
 * will apply them transitively backwards to obtain the token indices as they would have
 * been, had we run the search without symmetry. This gives us the following mapping:
 * original_indices:		This maps token indices from M'' to token indices from
 * 							M_0 (the initial marking).
 *
 * With this mapping, we can construct a non-symmetric trace that is equivalent to
 * the symmetric one which was found by the symbolic exploration.
 *
 * Participants:
 * A participant saves information about a particular token that participated in the transition
 * firing. It contains the following members:
 * 	- tokenIndex: this identifies a token from M in the schema above
 * 	- ti: The time interval (guard) that applied to token "tokenIndex"
 *  - placementAfterFiring: Since we don't save the actual markings that we encounter
 *  						during the search, we simply remember to which place this
 *  						token moves.
 *  - arcType: Indicates whether the arc was a normal arc or transport arc.
 *
 * Invariants:
 * Since we need to consider the invariants when computing concrete delays, we save them
 * together with the additional info about the transition firing. An invariant in this
 * context is a pair (i, inv), where i is an index of a token in M and inv is the invariant
 * from the place in which token i is located. We only store invariants which are different
 * from [0,inf).
 *
 * *NOTICE* When one wants to obtain the non-symmetric index from the index associated with
 * i in the pair above, one cannot use the original_indices mapping from the same TraceInfo
 * since this maps from M'' to original_indices. One must use the original_indices mapping
 * from the previous TraceInfo or map the index forward as seen in EntrySolver::RemapTokenIndex.
 */

namespace VerifyTAPN
{
	enum ArcType { NORMAL_ARC, TRANSPORT_ARC };

	class Participant
	{
	public:
		Participant(int tokenIndex, const TAPN::TimeInterval& ti, int placementAfterFiring, ArcType arcType)
			: tokenIndex(tokenIndex), ti(ti), placementAfterFiring(placementAfterFiring), arcType(arcType) { };
	public:
		int TokenIndex() const { return tokenIndex; };
		int PlacementAfterFiring() const { return placementAfterFiring; };
		inline const TAPN::TimeInterval& GetTimeInterval() const { return ti; };
		inline ArcType GetArcType() const { return arcType; };
	private:
		int tokenIndex;
		TAPN::TimeInterval ti;
		int placementAfterFiring;
		ArcType arcType;
	};

	class TraceInfo
	{
	public:
		typedef std::pair<unsigned int, TAPN::TimeInvariant> Invariant;
	public:
		TraceInfo(id_type prevState, int transitionIndex, id_type stateId)
			: stateId(stateId), prevState(prevState), transitionIndex(transitionIndex), symmetric_mapping(), original_indices(), transitionFiringMapping(), invariants() {};
	public:
		inline id_type StateId() const { return stateId; };
		inline id_type PreviousStateId() const { return prevState; };
		inline int	TransitionIndex() const { return transitionIndex; };
		inline const std::vector<Participant>& Participants() const { return participants; };
		inline std::vector<Participant>& Participants() { return participants; };
		inline const std::vector<Invariant>& GetInvariants() const { return invariants; };

		inline const IndirectionTable& GetSymmetricMapping() const { return symmetric_mapping; };
		inline IndirectionTable& GetSymmetricMapping() { return symmetric_mapping; };

		inline const std::vector<unsigned int>& GetOriginalMapping() const { return original_indices; };
		inline const IndirectionTable& GetTransitionFiringMapping() const { return transitionFiringMapping; };
	public:
		inline void AddParticipant(const Participant& participant) { participants.push_back(participant); };
		inline void SetSymmetricMapping(const IndirectionTable& indirectionTable) { this->symmetric_mapping = indirectionTable; };
		inline void SetOriginalMapping(const std::vector<unsigned int>& mapping) { original_indices = mapping; };
		inline void SetTransitionFiringMapping(const IndirectionTable& mapping) { transitionFiringMapping = mapping; };
		inline void SetInvariants(std::vector<Invariant>& invs) { invariants.swap(invs); };
	private: // data
		id_type stateId;
		id_type prevState;
		int transitionIndex;
		std::vector<Participant> participants;
		IndirectionTable symmetric_mapping;
		std::vector<unsigned int> original_indices;
		IndirectionTable transitionFiringMapping;
		std::vector<Invariant> invariants;
	};
}

#endif /* TRACEINFO_HPP_ */
