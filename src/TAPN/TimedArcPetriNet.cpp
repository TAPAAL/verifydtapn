#include "TimedArcPetriNet.hpp"

#include "TimeInterval.hpp"
#include <limits>

namespace VerifyTAPN {
	namespace TAPN {
		void TimedArcPetriNet::Initialize(bool useInfinityPlaces)
		{
			for(unsigned int i = 0; i < places.size(); i++){
				places[i]->SetIndex(i);
			}

			for(unsigned int i = 0; i < transitions.size(); i++){
				transitions[i]->SetIndex(i);
			}
//			placeIndices.set_empty_key(""); // assume place always have a name
//			IndexMap::value_type bottom(TimedPlace::Bottom().GetName(), TimedPlace::BottomIndex());
//			placeIndices.insert(bottom);
//			for(unsigned int i = 0; i < places.size(); i++){
//				IndexMap::value_type pair(places[i]->GetName(), i);
//				placeIndices.insert(pair);
//			}

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
			FindMaxConstants();

			if(useInfinityPlaces)
				MarkInfinityPlaces();
		}

		void TimedArcPetriNet::MarkInfinityPlaces()
		{
			for(TimedPlace::Vector::const_iterator iter = places.begin(); iter != places.end(); ++iter)
			{
				bool isInfinityPlace = true;
				for(TimedInputArc::WeakPtrVector::const_iterator arcIter = (*iter)->GetPostset().begin(); arcIter != (*iter)->GetPostset().end(); ++arcIter)
				{
					boost::shared_ptr<TimedInputArc> arc = arcIter->lock();

					if(!arc->Interval().IsZeroInfinity())
						isInfinityPlace = false;
				}

				(*iter)->MarkInfinityPlace(isInfinityPlace);
			}
		}

		void TimedArcPetriNet::FindMaxConstants()
		{
			for(TimedPlace::Vector::const_iterator iter = places.begin(); iter != places.end(); ++iter)
			{
				int maxConstant = 0;
				for(TimedInputArc::WeakPtrVector::const_iterator arcIter = (*iter)->GetPostset().begin(); arcIter != (*iter)->GetPostset().end(); ++arcIter)
				{
					boost::shared_ptr<TimedInputArc> ia = arcIter->lock();
					const TAPN::TimeInterval& interval = ia->Interval();

					const int lowerBound = interval.GetLowerBound();
					const int upperBound = interval.GetUpperBound();

					if(upperBound == std::numeric_limits<int>().max())
						maxConstant = (maxConstant < lowerBound ? lowerBound : maxConstant);
					else
						maxConstant = (maxConstant < upperBound ? upperBound : maxConstant);
				}
				(*iter)->SetMaxConstant(maxConstant);
			}
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
				const TimedTransition& t = *(*iter);
				Pairing p(*this, t);
				pairings.insert(std::pair<TimedTransition, Pairing>(t, p));
			}
		}
	}


}


