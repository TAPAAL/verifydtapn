#ifndef ABSTRACTMARKING_HPP_
#define ABSTRACTMARKING_HPP_

#include "../TAPN/TimeInterval.hpp"
#include "../TAPN/TimeInvariant.hpp"
#include <vector>
#include <list>
#include "../../typedefs.hpp"

namespace VerifyTAPN {
	class AbstractMarking {
	public:
		virtual ~AbstractMarking() { };

		//virtual AbstractMarking* Clone() const = 0;
		virtual id_type UniqueId() const = 0;
		virtual void MakeSymmetric(BiMap& indirectionTable) = 0; // Not sure this is the right place

		// Continuous part
		virtual void Reset(int token) = 0;
		virtual void Constrain(int token, const TAPN::TimeInterval& interval) = 0; // not sure if this should be here?
		virtual void Constrain(int token, const TAPN::TimeInvariant& invariant) = 0; // not sure if this should be here?
		virtual bool PotentiallySatisfies(int token, const TAPN::TimeInterval& interval) const = 0;

		// discrete part
		virtual void MoveToken(int tokenIndex, int newPlaceIndex) = 0;
		virtual void AddTokens(const std::list<int>& placeIndices) = 0; // TODO: make this more generic wrt. container
		virtual void RemoveTokens(const std::vector<int>& tokenIndices) = 0;
		virtual int GetTokenPlacement(int token) const = 0;
		virtual unsigned int NumberOfTokens() const = 0;
		virtual unsigned int NumberOfTokensInPlace(int placeIndex) const = 0;
	};

}

#endif /* ABSTRACTMARKING_HPP_ */
