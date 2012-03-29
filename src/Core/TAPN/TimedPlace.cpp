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

		void calculateCausality(TimedArcPetriNet* tapn, std::vector< TimedPlace >* result)
		{
			//TODO: husk der må ikke være cyckler!!!!
			result->push_back(*this);
			for(TransportArc::Vector::iterator iter = tapn->GetTransportArcs().begin(); iter != tapn->GetTransportArcs().end(); iter++){
				if((*iter)->Source() == *this){
					if((*iter)->Interval().GetUpperBound() == std::numeric_limits<int>().max()){

					}
				}
			}
		}
	}
}
