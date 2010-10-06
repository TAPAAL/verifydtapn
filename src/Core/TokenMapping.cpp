
#include "TokenMapping.hpp"

namespace VerifyTAPN {
	const int TokenMapping::GetMapping(int index) const
	{
		if(index > mapping.size()-1)
		{
			return std::numeric_limits<int>().max(); // return something that represents BOTTOM here.
		}
		else
		{
			return mapping[index];
		}
	}
}
