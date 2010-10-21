
#include "DiscretePart.hpp"
#include "../TAPN/TimedPlace.hpp"
#include <algorithm>
#include <exception>

namespace VerifyTAPN {

	const std::vector<int>& DiscretePart::GetTokenPlacementVector() const
	{
		return placement;
	}

	int DiscretePart::GetTokenPlacement(unsigned int tokenIndex) const
	{
		if(tokenIndex < 0 || tokenIndex > placement.size()-1)
			return -1; // Return BOTTOM - should not happen
		else
			return placement[tokenIndex];
	}

	int DiscretePart::IsTokenPresentInPlace(int placeIndex) const
	{
		int i = 0;
		for(std::vector<int>::const_iterator iter = placement.begin(); iter != placement.end(); ++iter)
		{
			if((*iter) == placeIndex)
			{
				return i;
			}
			i++;
		}

		return -1;
	}


	void DiscretePart::MoveToken(unsigned int tokenIndex, int newPlaceIndex)
	{
		assert(tokenIndex >= 0 && tokenIndex < placement.size());

		placement[tokenIndex] = newPlaceIndex;
	}

	int DiscretePart::MoveFirstTokenAtBottomTo(int newPlaceIndex)
	{
		int idx = -1;
		int size = placement.size();
		for(int i = 0; i < size; ++i)
		{
			if(placement[i] == TAPN::TimedPlace::BottomIndex())
			{
				idx = i;
				break;
			}
		}
		placement[idx] = newPlaceIndex;
		return idx;

	}

	int DiscretePart::NumberOfTokensInPlace(int placeIndex) const
	{
		int i = 0;
		for(std::vector<int>::const_iterator iter = placement.begin(); iter != placement.end(); ++iter)
		{
			if((*iter) == placeIndex)
			{
				i++;
			}
		}

		return i;
	}

	void DiscretePart::MakeKBound(unsigned int kBound)
	{
		while(placement.size() < kBound)
		{
			placement.push_back(TAPN::TimedPlace::BottomIndex());
		}
	}

}
