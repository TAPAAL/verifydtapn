#include "VerificationEngine.hpp"

namespace VerifyTAPN {

	const TimedArcPetriNet& VerificationEngine::GetModel() const
	{
		return tapn;
	}

	const VerificationOptions& VerificationEngine::GetVerificationOptions() const
	{
		return vo;
	}

	void VerificationEngine::Verify()
	{
		// TODO: verification pipeline, etc.
	}

}
