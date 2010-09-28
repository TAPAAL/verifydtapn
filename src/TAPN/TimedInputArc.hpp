#ifndef VERIFYTAPN_TAPN_TIMEDINPUTARC_HPP_
#define VERIFYTAPN_TAPN_TIMEDINPUTARC_HPP_

#include <vector>
#include "TimeInterval.hpp"
#include "TimedPlace.hpp"
#include "TimedTransition.hpp"
#include "boost/smart_ptr.hpp"

namespace VerifyTAPN {
	namespace TAPN {

		class TimedInputArc
		{
		public: // typedefs
			typedef std::vector< boost::shared_ptr<TimedInputArc> > Vector;
		public:
			TimedInputArc(const TimedPlace& place, const TimedTransition& transition, const TimeInterval& interval) : interval(interval), place(place), transition(transition) { };
			virtual ~TimedInputArc() { /* empty */}

		public: // Inspectors
			void Print(std::ostream& out) const;
		private:
			const TimeInterval interval;
			const TimedPlace& place;
			const TimedTransition& transition;
		};

		inline std::ostream& operator<<(std::ostream& out, const TimedInputArc& arc)
		{
			arc.Print(out);
			return out;
		}
	}
}

#endif /* VERIFYTAPN_TAPN_TIMEDINPUTARC_HPP_ */
