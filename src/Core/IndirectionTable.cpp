#include "IndirectionTable.hpp"
#include <iostream>

namespace VerifyTAPN
{
	void IndirectionTable::Print(std::ostream& out) const
	{
		out << "IndirectionTable: ";
		typedef BiMap::left_const_iterator const_iterator;
		for( const_iterator iter = map.left.begin(), iend = map.left.end(); iter != iend; ++iter )
		{
			std::cout << iter->first << ":" << iter->second << ", ";
		}
	}

	std::ostream& operator<<(std::ostream& out, const IndirectionTable& t)
	{
		t.Print(out);
		return out;
	}
}
