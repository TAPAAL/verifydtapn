#ifndef VERIFICATIONENGINE_HPP_
#define VERIFICATIONENGINE_HPP_

#include "../TAPN/TimedArcPetriNet.hpp"
#include "VerificationOptions.hpp"

using namespace VerifyTAPN::TAPN;

namespace VerifyTAPN {
	class VerificationEngine {
		public: // construction
			VerificationEngine(const TimedArcPetriNet& tapn, const VerificationOptions& vo) : tapn(tapn), vo(vo) { };
			virtual ~VerificationEngine() { };

		public:
			void Verify();

		public: // inspectors
			const TimedArcPetriNet& GetModel() const;
			const VerificationOptions& GetVerificationOptions() const;

		private: // data
			TimedArcPetriNet tapn;
			VerificationOptions vo;
	};
}
#endif /* VERIFICATIONENGINE_HPP_ */
