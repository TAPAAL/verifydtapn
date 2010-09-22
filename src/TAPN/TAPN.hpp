#ifndef VERIFYTAPN_TAPN_TAPN_HPP_
#define VERIFYTAPN_TAPN_TAPN_HPP_

#include "TimedPlace.hpp"
#include "TimedTransition.hpp"
#include "TimedInputArc.hpp"
#include "OutputArc.hpp"

namespace VerifyTAPN {
	namespace TAPN {

		class TAPN
		{
		public:// construction
			TAPN();
			virtual ~TAPN() { /* empty */ }

		public: // modifiers
			void initialize();
		private: // data
			const TimedPlace::Vector places;
			const TimedTransition::Vector transitions;
			const TimedInputArc::Vector inputArcs;
			const OutputArc::Vector outputArcs;
		};
	}
}

#endif /* VERIFYTAPN_TAPN_TAPN_HPP_ */
