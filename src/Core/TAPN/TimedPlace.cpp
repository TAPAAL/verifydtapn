#include "TimedPlace.hpp"

namespace VerifyTAPN{
	namespace TAPN{
		const std::string TimedPlace::BOTTOM_NAME = "*BOTTOM*";

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
			out << "(" << name << " (index: " << index << "), " << timeInvariant << ", Max Constant: " << maxConstant << ", Infinity Place: " << (isUntimed ? "true" : "false") << ")";
		}
	}
}
