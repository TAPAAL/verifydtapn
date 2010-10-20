
#include "TokenMapping.hpp"

namespace VerifyTAPN {
	const int TokenMapping::GetMapping(int index) const
	{
		if(index > mapping.size()-1)
		{
			return -1; // return something that represents BOTTOM here.
		}
		else
		{
			return mapping[index];
		}
	}

	void TokenMapping::AddTokenToMapping(int tokenIndex)
	{
		mapping.push_back(tokenIndex);
	}
}
