#include "AbstractMarking.hpp"
#include <iostream>

namespace VerifyTAPN
{
	std::ostream& operator<<(std::ostream& out, const AbstractMarking& m)
	{
		m.Print(out);
		return out;
	};
}
