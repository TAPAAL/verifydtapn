#include "TimedPlace.hpp"

namespace VerifyTAPN{
	namespace TAPN{
		const std::string& TimedPlace::GetName() const
		{
			return name;
		}

	    void TimedPlace::Print(std::ostream& out) const
	    {
	    	out << GetName();
	    }
	}
}
