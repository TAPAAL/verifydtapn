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

		void TimedTransition::AddToPreset(const boost::shared_ptr<TimedInputArc>& arc)
		{
			preset.push_back(arc);
		}
	}
}




