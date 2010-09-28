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

			out << std::endl << "  Input Arcs: ";
			for(TimedInputArc::Vector::iterator iter = inputArcs->begin();iter != inputArcs->end();iter++){
				out << *(*iter);
				out << ", ";
			}

			out << std::endl;
		}

		const TimedPlace& TimedArcPetriNet::FindPlaceByName(const std::string& name) const
		{
			for(TimedPlace::Vector::iterator iter = places->begin(); iter != places->end(); iter++)
			{
				if((*iter)->GetName() == name){
					return *(*iter);
				}
			}

			throw new std::exception;
		}

		bool TimedArcPetriNet::ContainsPlaceWithName(const std::string& name) const
		{
			bool result = false;
			for(TimedPlace::Vector::iterator iter = places->begin(); iter != places->end(); iter++)
			{
				if((*iter)->GetName() == name){
					result = true;
					break;
				}
			}

			return result;
		}
	}
}


