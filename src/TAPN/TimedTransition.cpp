#include "TimedTransition.hpp"
#include "../Core/SymMarking.hpp"
#include "../Core/DiscretePart.hpp"
#include "TimedArcPetriNet.hpp"

namespace VerifyTAPN {
	namespace TAPN {
		const std::string& TimedTransition::GetName() const
		{
			return name;
		}

		void TimedTransition::Print(std::ostream& out) const
		{
			out << GetName();
		}

		void TimedTransition::AddToPreset(const boost::shared_ptr<TimedInputArc>& arc)
		{
			if(arc)
			{
				preset.push_back(arc);
			}
		}

		void TimedTransition::AddToPostset(const boost::shared_ptr<OutputArc>& arc)
		{
			if(arc)
			{
				postset.push_back(arc);
			}
		}

//		bool TimedTransition::isEnabledBy(const TimedArcPetriNet& tapn, const VerifyTAPN::SymMarking& marking) const
//		{
//			for(TimedInputArc::WeakPtrVector::const_iterator iter = preset.begin(); iter != preset.end(); ++iter)
//			{
//
//				TokenMapping map = marking.GetTokenMapping();
//				boost::shared_ptr<TimedInputArc> tiaPtr = (*iter).lock();
//				if(tiaPtr)
//				{
//					bool notAppropriateAge = true;
//					for(int i = 1; i < map.size(); ++i)
//					{
//						// map dbm index i to a token index in the placement vector and get the placeIndex of that token
//						int placeIndex = (marking.GetDiscretePart())->GetTokenPlacement(map.GetMapping(i));
//
//						if(placeIndex >= 0 && placeIndex == tapn.GetPlaceIndex(tiaPtr->InputPlace())) // token is in the net and in the input place of the current arc
//						{
//							// check for the age of token.
//							// the time interval on an input arc corresponds to a conjunction of guards
//							// i.e. [2,3) corresponds to x >= 2 && x < 3.
//							TimeInterval ti = tiaPtr->Interval();
//
//
//							// Note that dbm::satifies cannot be used to check conjunction according to the documentation - only disjunction.
//							// thus for [2,3) we check !(x >= 2) || !(x < 3) which corresponds to !(x >= 2 && x < 3)
//
//							// check lower bound
//							bool isLowerBoundSat = marking.GetZone().satisfies(0,i,ti.LowerBoundToDBMRaw());
//
//							// check upper bound - TODO: this is returns true even if dbm.setZero() and satisfies(i,0, < inf)
//							bool isUpperBoundSat = marking.GetZone().satisfies(i,0, ti.UpperBoundToDBMRaw());
//
//							notAppropriateAge = !isLowerBoundSat || !isUpperBoundSat;
//						}
//
//						if(!notAppropriateAge){ // token of appropriate age was found for the current input place
//							break;
//						}
//
//					}
//
//					if(notAppropriateAge){ // no token was found of appropriate age for the current input place
//						return false;
//					}
//				}
//				else
//				{
//					return false;
//				}
//
//			}
//
//			return true;
//		}

	}
}




