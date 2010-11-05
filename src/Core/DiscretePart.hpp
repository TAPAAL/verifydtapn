#ifndef DISCRETEPART_HPP_
#define DISCRETEPART_HPP_

#include <vector>
#include "boost/functional/hash.hpp"
#include "../HashFunctions/MurmurHash2Neutral.hpp"
#include <iostream>

namespace VerifyTAPN {
	struct hash;
	struct eqdp;

	class DiscretePart {
		friend struct VerifyTAPN::hash;
		friend struct VerifyTAPN::eqdp;
	public: // construction
		DiscretePart(const std::vector<int>& placement) : placement(placement) { };
		explicit DiscretePart(const DiscretePart& dp) : placement(dp.placement) { };

		virtual ~DiscretePart() { };

	public: // inspectors
		inline int GetTokenPlacement(unsigned int tokenIndex) const { return placement[tokenIndex]; }
		int IsTokenPresentInPlace(int placeIndex) const;
		int NumberOfTokensInPlace(int placeIndex) const;
		const std::vector<int>& GetTokenPlacementVector() const;
		inline const unsigned int size() const { return placement.size(); }


	public: // modifiers
		void MoveToken(unsigned int tokenIndex, int newPlaceIndex);
		int MoveFirstTokenAtBottomTo(int newPlaceIndex);
		void RemoveToken(int index) { placement.erase(placement.begin() + index); }
		void AddTokenInPlace(int placeIndex) { placement.push_back(placeIndex); }
		void MakeKBound(unsigned int kBound);

	private: // data
		std::vector<int> placement;
	};

	struct hash : public std::unary_function<const VerifyTAPN::DiscretePart*, size_t>
	{
		size_t operator()(const VerifyTAPN::DiscretePart* const& dp) const
		{
			size_t hash = MurmurHashNeutral2(dp->placement, 0xdeadbeef);//boost::hash_range(dp->placement.begin(), dp->placement.end());
			return hash;
		}
	};

	struct eqdp : public std::binary_function<const VerifyTAPN::DiscretePart*, const VerifyTAPN::DiscretePart*, bool>
	{
		bool operator()(const VerifyTAPN::DiscretePart* const& dp1, const VerifyTAPN::DiscretePart* const& dp2) const
		{
			return dp1->placement == dp2->placement;
		}
	};

}

#endif /* DISCRETEPART_HPP_ */
