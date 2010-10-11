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

			MakeTAPNConservative();
		}

		void TimedArcPetriNet::MakeTAPNConservative()
		{
			for(TimedTransition::Vector::const_iterator iter = transitions.begin(); iter != transitions.end(); ++iter)
			{
				if(!(*iter)->isConservative())
				{
					int diff = std::abs((*iter)->GetPresetSize() - (*iter)->GetPostsetSize());

					if((*iter)->GetPresetSize() > (*iter)->GetPostsetSize())
					{
						while(diff > 0)
						{
//							boost::shared_ptr<OutputArc> bottomArc = boost::make_shared<OutputArc>((*(*iter)), TimedPlace::Bottom());
//							(*iter)->AddToPostset(bottomArc);
							diff--;
						}
					}
					else
					{
						// add bottom to preset until t is conservative
						while(diff > 0)
						{
//							boost::shared_ptr<TimedInputArc> bottomArc = boost::make_shared<TimedInputArc>(TimedPlace::Bottom(), (*iter));
//							(*iter)->AddToPreset(bottomArc);

							diff--;
						}
					}

				}
			}
		}

		int TimedArcPetriNet::GetPlaceIndex(const TimedPlace& p) const
		{
			int idx = -1;
			for(unsigned int i = 0; i < places.size(); ++i)
			{
				if(places[i]->GetName() == p.GetName())
				{
					idx = i;
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
	}
}


