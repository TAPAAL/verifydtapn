#include "StoredMarking.hpp"
#include <iostream>

namespace VerifyTAPN
{
	std::ostream& operator<<(std::ostream& out, const StoredMarking& m)
	{
		m.Print(out);
		return out;
	}
}
