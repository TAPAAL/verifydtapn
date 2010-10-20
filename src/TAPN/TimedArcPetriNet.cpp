#include "TimedArcPetriNet.hpp"

namespace VerifyTAPN {
	namespace TAPN {
		void TimedArcPetriNet::Initialize()
		{
			for(TimedInputArc::Vector::const_iterator iter = inputArcs.begin(); iter != inputArcs.end(); ++iter)
			{
				const boost::shared_ptr<TimedInputArc>& arc = *iter;
				arc->InputPlace().AddToPostset(arc);
				arc->OutputTransition().AddToPreset(arc);
			}

			for(OutputArc::Vector::const_iterator iter = outputArcs.begin(); iter != outputArcs.end(); ++iter)
			{
				const boost::shared_ptr<OutputArc>& arc = *iter;
				arc->OutputPlace().AddToPreset(arc);
				arc->InputTransition().AddToPostset(arc);
			}

			GeneratePairings();
		}

		int TimedArcPetriNet::GetPlaceIndex(const TimedPlace& p) const
		{
			return GetPlaceIndex(p.GetName());
		}

		int TimedArcPetriNet::GetPlaceIndex(const std::string& placeName) const
			{
				int idx = TimedPlace::BottomIndex();
				for(unsigned int i = 0; i < places.size(); ++i)
				{
					if(places[i]->GetName() == placeName)
					{
						idx = i;
						break;
					}
				}

				return idx;
			}

		void TimedArcPetriNet::Print(std::ostream & out) const
		{
			out << "TAPN:" << std::endl << "  Places: ";
			for(TimedPlace::Vector::const_iterator iter = places.begin();iter != places.end();iter++)
			{
				out << *(*iter);
				out << std::endl;
			}

			out << std::endl << "  Transitions: ";
			for(TimedTransition::Vector::const_iterator iter = transitions.begin(); iter != transitions.end(); iter++)
			{
				out << *(*iter);
				out << std::endl;
			}

			out << std::endl << "  Input Arcs: ";
			for(TimedInputArc::Vector::const_iterator iter = inputArcs.begin(); iter != inputArcs.end(); iter++)
			{
				out << *(*iter);
				out << ", ";
			}

			out << std::endl << "  Output Arcs: ";
			for(OutputArc::Vector::const_iterator iter = outputArcs.begin(); iter != outputArcs.end(); iter++)
			{
				out << *(*iter);
				out << ", ";
			}

			out << std::endl;
		}


		void TimedArcPetriNet::GeneratePairings()
		{
			for(TimedTransition::Vector::const_iterator iter = transitions.begin(); iter != transitions.end(); ++iter)
			{
				TimedTransition t = *(*iter);
				Pairing p(t);
				pairings[t] = p;
			}
		}
	}


}


