#ifndef DISCRETEMARKING_HPP_
#define DISCRETEMARKING_HPP_

#include "SymbolicMarking.hpp"
#include "DiscretePart.hpp"

namespace VerifyTAPN {

	class DiscreteMarking : public SymbolicMarking {
	public:
		DiscreteMarking(const DiscretePart& dp) : dp(dp) { };
		DiscreteMarking(const DiscreteMarking& dm) : dp(dm.dp) { };
		virtual ~DiscreteMarking() { };

	public:
		virtual void MoveToken(int token, int targetPlace) { dp.MoveToken(token, targetPlace); };
		virtual int NumberOfTokens() const { return dp.size(); };

		virtual void AddTokens(const std::vector<int>& placeIndices)
		{
			for(std::vector<int>::const_iterator i = placeIndices.begin(); i != placeIndices.end(); i++)
			{
				dp.AddTokenInPlace(*i);
			}
		};

		virtual void RemoveTokens(const std::vector<int>& tokenIndices)
		{
			for(std::vector<int>::const_iterator i = tokenIndices.begin(); i != tokenIndices.end(); i++)
			{
				dp.RemoveToken(*i);
			}
		};

	private: // data
		DiscretePart dp;
	};

}

#endif /* DISCRETEMARKING_HPP_ */
