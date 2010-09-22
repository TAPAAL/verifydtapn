#include "TimedTransition.hpp"

namespace VerifyTAPN {
	namespace TAPN {
		const std::string& TimedTransition::GetName() const
		{
			return name;
		}

		void TimedTransition::Print(std::ostream& out) const
		{
			out << GetName();
		}
	}
}




