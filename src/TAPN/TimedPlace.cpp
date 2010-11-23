#include "TimedPlace.hpp"

namespace VerifyTAPN{
	namespace TAPN{
		const std::string& TimedPlace::GetName() const
		{
			return name;
		}

		const std::string& TimedPlace::GetId() const
		{
			return id;
		}

		void TimedPlace::Print(std::ostream& out) const
		{
			out << "(" << name << " (index: " << index << "), " << timeInvariant << ", Max Constant: " << maxConstant << ", Infinity Place: " << (isInfinityPlace ? "true" : "false") << ")";
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
