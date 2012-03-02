#ifndef DISCRETEPART_HPP_
#define DISCRETEPART_HPP_

#include <vector>
#include "boost/functional/hash.hpp"
#include "../../HashFunctions/MurmurHash2Neutral.hpp"
#include <iostream>

namespace VerifyTAPN {
	struct hash;
	struct eqdp;

	class DiscretePart {
		friend struct VerifyTAPN::hash;
		friend struct VerifyTAPN::eqdp;
	public: // construction
		DiscretePart() : placement() { };
		explicit DiscretePart(const std::vector<int>& placement) : placement(placement) { };
		DiscretePart(const DiscretePart& dp) : placement(dp.placement) { };
		DiscretePart& operator=(const DiscretePart& dp)
		{
			placement = dp.placement;
			return *this;
		}
		virtual ~DiscretePart() { };

	public: // inspectors
		inline int GetTokenPlacement(unsigned int tokenIndex) const { return placement[tokenIndex]; }
		int NumberOfTokensInPlace(int placeIndex) const;
		const std::vector<int>& GetTokenPlacementVector() const { return placement; }
		inline const unsigned int size() const { return placement.size(); }


	public: // modifiers
		void MoveToken(unsigned int tokenIndex, int newPlaceIndex);
		void RemoveToken(int index) { placement.erase(placement.begin() + index); }
		void AddTokenInPlace(int placeIndex) { placement.push_back(placeIndex); }
		void Swap(int tokenI, int tokenJ);

	private: // data
		std::vector<int> placement;
	};

	struct hash : public std::unary_function<const VerifyTAPN::DiscretePart, size_t>
	{
		size_t operator()(const VerifyTAPN::DiscretePart& dp) const
		{
			size_t hash = boost::hash_range(dp.placement.begin(), dp.placement.end());
			return hash;
		}
	};

	struct eqdp : public std::binary_function<const VerifyTAPN::DiscretePart, const VerifyTAPN::DiscretePart, bool>
	{
		bool operator()(const VerifyTAPN::DiscretePart& dp1, const VerifyTAPN::DiscretePart& dp2) const
		{
			return dp1.placement == dp2.placement;
		}
	};

}

#endif /* DISCRETEPART_HPP_ */
