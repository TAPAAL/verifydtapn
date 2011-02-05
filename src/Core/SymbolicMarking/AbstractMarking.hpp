#ifndef ABSTRACTMARKING_HPP_
#define ABSTRACTMARKING_HPP_

#include "../TAPN/TimeInterval.hpp"

namespace VerifyTAPN {

	class AbstractMarking {
	public:
		virtual ~AbstractMarking() { };

		virtual AbstractMarking* Clone() const = 0;
		//virtual void Canonicalize() = 0;

		// Continuous part
		virtual void Reset(int token) = 0;
		virtual void Constrain(int token, const TAPN::TimeInterval& interval) = 0; // not sure if this should be here?
		virtual bool Satisfies(int token, const TAPN::TimeInterval& interval) const = 0;

		// discrete part
		virtual void MoveToken(int tokenIndex, int newPlaceIndex) = 0;
		virtual void AddTokens(const std::vector<int>& placeIndices) = 0;
		virtual void RemoveTokens(const std::vector<int>& tokenIndices) = 0;
		virtual int NumberOfTokens() const = 0;
	};

}

#endif /* ABSTRACTMARKING_HPP_ */
