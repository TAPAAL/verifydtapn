#ifndef DISCRETEPART_HPP_
#define DISCRETEPART_HPP_

#include <vector>
#include "boost/functional/hash.hpp"

namespace VerifyTAPN {
	struct hash;

	class DiscretePart {
		friend struct VerifyTAPN::hash;
	public: // construction
		DiscretePart(const std::vector<int>& placement) : placement(placement) { };
		explicit DiscretePart(const DiscretePart& dp) : placement(dp.placement) { };

		virtual ~DiscretePart() { };

	public: // inspectors
		int GetTokenPlacement(int tokenIndex) const;
		int IsTokenPresentInPlace(int placeIndex) const;
		int NumberOfTokensInPlace(int placeIndex) const;
		const std::vector<int>& GetTokenPlacementVector() const;

	public: // modifiers
		void MoveToken(int tokenIndex, int newPlaceIndex);
		int MoveFirstTokenAtBottomTo(int newPlaceIndex);
		void MakeKBound(int kBound);

	private: // data
		std::vector<int> placement;

	};

	struct hash : public std::unary_function<const VerifyTAPN::DiscretePart*, size_t>
		{
			size_t operator()(const VerifyTAPN::DiscretePart* const& dp) const
			{
				size_t hash = boost::hash_range(dp->placement.begin(), dp->placement.end());
				return hash;
			}
		};

}

#endif /* DISCRETEPART_HPP_ */
