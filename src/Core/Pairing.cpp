#include "Pairing.hpp"
#include "../TAPN/TimedTransition.hpp"
#include "../TAPN/TimedArcPetriNet.hpp"

namespace VerifyTAPN {

using namespace TAPN;

	void Pairing::GeneratePairingFor(const TimedArcPetriNet& tapn, const TAPN::TimedTransition& t) {
		TimedInputArc::WeakPtrVector preset = t.GetPreset();
		OutputArc::WeakPtrVector postset = t.GetPostset();

		unsigned int sizeOfPairing = preset.size() >= postset.size() ? preset.size() : postset.size();

		int inputPlace;
		int outputPlace;

		for(unsigned int i = 0; i < sizeOfPairing; i++)
		{
			if(i < preset.size() && i < postset.size())
			{
				boost::shared_ptr<TimedInputArc> tiaPtr = preset[i].lock();
				boost::shared_ptr<OutputArc> oaPtr = postset[i].lock();

				inputPlace = tapn.GetPlaceIndex(tiaPtr->InputPlace());
				outputPlace = tapn.GetPlaceIndex(oaPtr->OutputPlace());

				Add(inputPlace, outputPlace);
			}
			else if(i < preset.size() && i >= postset.size()){
				boost::shared_ptr<TimedInputArc> tiaPtr = preset[i].lock();

				inputPlace = tapn.GetPlaceIndex(tiaPtr->InputPlace());
				Add(inputPlace, TimedPlace::BottomIndex());
			}
			else if(i >= preset.size() && i < postset.size())
			{
				boost::shared_ptr<OutputArc> oaPtr = postset[i].lock();

				outputPlace = tapn.GetPlaceIndex(oaPtr->OutputPlace());

				Add(TimedPlace::BottomIndex(),outputPlace);
			}
		}
	}

	const std::list<int>& Pairing::GetOutputPlacesFor(int inputPlace) const
	{
		return pairing[inputPlace];
	}

	void Pairing::Add(int inputPlace, int outputPlace)
	{
		std::list<int>& outPlaces = pairing[inputPlace];

		outPlaces.push_back(outputPlace);
	}

	void Pairing::Print(std::ostream& out) const
	{
		out << "( INPUT PLACE, OUTPUT PLACE )\n";
		out << "-------------------------------\n";
		for(HashMap::const_iterator iter = pairing.begin(); iter != pairing.end(); ++iter)
		{
			std::list<int> outPlaces = (*iter).second;
			for(std::list<int>::const_iterator pIter = outPlaces.begin(); pIter != outPlaces.end(); ++pIter)
			{
				out << "(" << (*iter).first << ", " << (*pIter) << ")" << "\n";
			}
		}

		out << "\n";
	}
}
