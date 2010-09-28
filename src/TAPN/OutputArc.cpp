#include "OutputArc.hpp"

namespace VerifyTAPN {
	namespace TAPN {
		void OutputArc::Print(std::ostream& out) const
		{
			out << "From " << transition.GetName() << " to " << place.GetName();
		}
	}
}
