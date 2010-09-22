#ifndef VERIFYTAPN_TAPN_TIMEDINPUTARC_HPP_
#define VERIFYTAPN_TAPN_TIMEDINPUTARC_HPP_

#include <vector>
#include "TimeInterval.hpp"

namespace VerifyTAPN {
	namespace TAPN {

		class TimedInputArc
		{
		public: // typedefs
			typedef std::vector<TimedInputArc> Vector; // probably needs to be pointers?
		public:
			TimedInputArc(const TimeInterval& interval) : interval(interval) { };
			virtual ~TimedInputArc() { /* empty */}

		private:
			const TimeInterval interval;
		};
	}
}

#endif /* VERIFYTAPN_TAPN_TIMEDINPUTARC_HPP_ */
