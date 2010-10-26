#include "TimedArcPetriNet.hpp"

#include "TimeInterval.hpp"
#include <limits>

namespace VerifyTAPN {
	namespace TAPN {
		void TimedArcPetriNet::Initialize()
		{
			placeIndices.set_empty_key(""); // assume place always have a name
			IndexMap::value_type bottom(TimedPlace::Bottom().GetName(), TimedPlace::BottomIndex());
			placeIndices.insert(bottom);
			for(unsigned int i = 0; i < places.size(); i++){
				IndexMap::value_type pair(places[i]->GetName(), i);
				placeIndices.insert(pair);
			}

			for(TimedInputArc::Vector::const_iterator iter = inputArcs.begin(); iter != inputArcs.end(); ++iter)
			{
				const boost::shared_ptr<TimedInputArc>& arc = *iter;
				arc->InputPlace().AddToPostset(arc);
				arc->OutputTransition().AddToPreset(arc);
				UpdateMaxConstant(arc->Interval());
			}

			for(OutputArc::Vector::const_iterator iter = outputArcs.begin(); iter != outputArcs.end(); ++iter)
			{
				const boost::shared_ptr<OutputArc>& arc = *iter;
				arc->OutputPlace().AddToPreset(arc);
				arc->InputTransition().AddToPostset(arc);
			}

			GeneratePairings();
		}

		void TimedArcPetriNet::UpdateMaxConstant(const TimeInterval& interval)
		{
			int lowerBound = interval.GetLowerBound();
			int upperBound = interval.GetUpperBound();
			if(lowerBound < std::numeric_limits<int>().max() && lowerBound > maxConstant)
			{
				maxConstant = lowerBound;
			}
			if(upperBound < std::numeric_limits<int>().max() && upperBound > maxConstant)
			{
				maxConstant = upperBound;
			}
		}

		int TimedArcPetriNet::GetPlaceIndex(const TimedPlace& p) const
		{
			return GetPlaceIndex(p.GetName());
		}

		int TimedArcPetriNet::GetPlaceIndex(const std::string& placeName) const
		{
			IndexMap::const_iterator iter = placeIndices.find(placeName);
			assert(iter != placeIndices.end());
			return iter->second;
//				int idx = TimedPlace::BottomIndex();
//				for(unsigned int i = 0; i < places.size(); ++i)
//				{
//					if(places[i]->GetName() == placeName)
//					{
//						idx = i;
//						break;
//					}
//				}
//
//				return idx;
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


