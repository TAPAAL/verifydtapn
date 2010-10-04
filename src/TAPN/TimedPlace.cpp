#include "TimedPlace.hpp"

namespace VerifyTAPN{
	namespace TAPN{
		const std::string& TimedPlace::GetName() const
		{
			return name;
		}

		void TimedPlace::Print(std::ostream& out) const
		{
			out << "(" << name << ", " << timeInvariant << ")";
		}

		void TimedPlace::AddToPostset(const boost::shared_ptr<TimedInputArc>& arc)
		{
			if(arc)
			{
				postset.push_back(arc);
			}
		}

		void TimedPlace::AddToPreset(const boost::shared_ptr<OutputArc>& arc)
		{
			if(arc)
			{
				preset.push_back(arc);
			}
		}
	}
}
