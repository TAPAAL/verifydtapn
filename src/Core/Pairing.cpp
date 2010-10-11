#include "Pairing.hpp"
#include "../TAPN/TimedTransition.hpp"

namespace VerifyTAPN {

using namespace TAPN;

	void Pairing::GeneratePairingFor(const TAPN::TimedTransition& t) {
		TimedInputArc::WeakPtrVector preset = t.GetPreset();
		OutputArc::WeakPtrVector postset = t.GetPostset();

		unsigned int sizeOfPairing = preset.size() >= postset.size() ? preset.size() : postset.size();

		TimedPlace inputPlace;
		TimedPlace outputPlace;

		for(unsigned int i = 0; i < sizeOfPairing; i++)
		{
			if(i < preset.size() && i < postset.size())
			{
				boost::shared_ptr<TimedInputArc> tiaPtr = preset[i].lock();
				boost::shared_ptr<OutputArc> oaPtr = postset[i].lock();

				inputPlace = tiaPtr->InputPlace();
				outputPlace = oaPtr->OutputPlace();

				Add(inputPlace,outputPlace);
			}
			else if(i < preset.size() && i >= postset.size()){
				boost::shared_ptr<TimedInputArc> tiaPtr = preset[i].lock();

				inputPlace = tiaPtr->InputPlace();
				Add(inputPlace,TimedPlace::Bottom());
			}
			else if(i >= preset.size() && i < postset.size())
			{
				boost::shared_ptr<OutputArc> oaPtr = postset[i].lock();

				outputPlace = oaPtr->OutputPlace();

				Add(TimedPlace::Bottom(),outputPlace);
			}
			else{
				// Should Not Happen

			}
		}
	}

	std::list<TimedPlace>& Pairing::GetOutputPlaceFor(const TimedPlace& inputPlace)
	{
		return pairing[inputPlace];
	}

	void Pairing::Add(const TimedPlace& inputPlace, const TimedPlace& outputPlace)
	{
		std::list<TimedPlace>& outPlaces = pairing[inputPlace];

		outPlaces.push_back(outputPlace);
	}

	void Pairing::Print(std::ostream& out) const
	{
		out << "( INPUT PLACE, OUTPUT PLACE )\n";
		out << "-------------------------------\n";
		for(HashMap::const_iterator iter = pairing.begin(); iter != pairing.end(); ++iter)
		{
			std::list<TimedPlace> outPlaces = (*iter).second;
			for(std::list<TimedPlace>::const_iterator pIter = outPlaces.begin(); pIter != outPlaces.end(); ++pIter)
			{
				out << "(" << (*iter).first << ", " << (*pIter) << ")" << "\n";
			}
		}

		out << "\n";
	}
}
