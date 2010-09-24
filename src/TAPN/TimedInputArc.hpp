#ifndef VERIFYTAPN_TAPN_TIMEDINPUTARC_HPP_
#define VERIFYTAPN_TAPN_TIMEDINPUTARC_HPP_

#include <vector>
#include "TimeInterval.hpp"
#include "boost/smart_ptr.hpp"

namespace VerifyTAPN {
	namespace TAPN {

		class TimedInputArc
		{
		public: // typedefs
			typedef std::vector< boost::shared_ptr<TimedInputArc> > Vector;
		public:
			TimedInputArc(const TimeInterval& interval) : interval(interval) { };
			virtual ~TimedInputArc() { /* empty */}

		private:
			const TimeInterval interval;
		};
	}
}

#endif /* VERIFYTAPN_TAPN_TIMEDINPUTARC_HPP_ */
