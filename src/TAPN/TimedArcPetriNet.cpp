#include "TimedArcPetriNet.hpp"

namespace VerifyTAPN {
	namespace TAPN {
		void TimedArcPetriNet::Initialize()
		{
		}

		void TimedArcPetriNet::Print(std::ostream & out) const
		{
			out << "TAPN:" << std::endl << "  Places: ";
			for(TimedPlace::Vector::iterator iter = places->begin();iter != places->end();iter++){
				out << *(*iter);
				out << ", ";
			}
			out << std::endl << "  Transitions: ";
			for(TimedTransition::Vector::iterator iter = transitions->begin();iter != transitions->end();iter++){
				out << *(*iter);
				out << ", ";
			}
			out << std::endl;
		}
	}
}


