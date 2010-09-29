#include "TimedArcPetriNet.hpp"
#include "boost/bind.hpp"

namespace VerifyTAPN {
	namespace TAPN {
		void TimedArcPetriNet::Initialize()
		{
		}

		void TimedArcPetriNet::AddPlace(TimedPlace* place)
		{
			places.push_back(place);
		}

		void TimedArcPetriNet::AddTransition(TimedTransition* transition)
		{
			transitions.push_back(transition);
		}

		void TimedArcPetriNet::AddInputArc(TimedInputArc* arc)
		{
			inputArcs.push_back(arc);
		}

		void TimedArcPetriNet::AddOutputArc(OutputArc* arc)
		{
			outputArcs.push_back(arc);
		}

		const TimedPlace& TimedArcPetriNet::FindPlaceByName(const std::string& name) const
		{
			TimedPlace::Vector::const_iterator place = find_if(places.begin(), places.end(), boost::bind(boost::mem_fn(&TimedPlace::GetName), _1) == name);
			return *place;
		}

		const TimedTransition& TimedArcPetriNet::FindTransitionByName(const std::string& name) const
		{
			TimedTransition::Vector::const_iterator transition = find_if(transitions.begin(), transitions.end(), boost::bind(boost::mem_fn(&TimedTransition::GetName), _1) == name);
			return *transition;
		}

		bool TimedArcPetriNet::ContainsPlaceByName(const std::string& name) const
		{
			TimedPlace::Vector::const_iterator place = find_if(places.begin(), places.end(), boost::bind(boost::mem_fn(&TimedPlace::GetName), _1) == name);
			return place != places.end();
		}

		void TimedArcPetriNet::Print(std::ostream & out) const
		{
			out << "TAPN:" << std::endl << "  Places: ";
			for(TimedPlace::Vector::const_iterator iter = places.begin(); iter != places.end(); iter++){
				out << *iter;
				out << ", ";
			}

			out << std::endl << "  Transitions: ";
			for(TimedTransition::Vector::const_iterator iter = transitions.begin(); iter != transitions.end(); iter++){
				out << *iter;
				out << ", ";
			}

			out << std::endl << "  Input Arcs: ";
			for(TimedInputArc::Vector::const_iterator iter = inputArcs.begin(); iter != inputArcs.end(); iter++){
				out << *iter;
				out << ", ";
			}

			out << std::endl << "  Output Arcs: ";
			for(OutputArc::Vector::const_iterator iter = outputArcs.begin(); iter != outputArcs.end(); iter++){
				out << *iter;
				out << ", ";
			}

			out << std::endl;
		}
	}
}


