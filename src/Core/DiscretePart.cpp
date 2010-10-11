
#include "DiscretePart.hpp"

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

}
