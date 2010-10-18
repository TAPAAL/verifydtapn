
#include "DiscretePart.hpp"
#include "../TAPN/TimedPlace.hpp"
#include <algorithm>
#include <exception>

namespace VerifyTAPN {

	const std::vector<int>& DiscretePart::GetTokenPlacementVector() const
	{
		return placement;
	}

	int DiscretePart::GetTokenPlacement(int tokenIndex) const
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


	void DiscretePart::MoveToken(int tokenIndex, int newPlaceIndex)
	{
		assert(tokenIndex >= 0 && tokenIndex < placement.size());

		placement[tokenIndex] = newPlaceIndex;
	}

	void DiscretePart::MoveFirstTokenAtBottomTo(int newPlaceIndex)
	{
		std::vector<int>::iterator i = std::find( placement.begin(), placement.end(), VerifyTAPN::TAPN::TimedPlace::BottomIndex());

		assert(i != placement.end());

		*i = newPlaceIndex;

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

}
