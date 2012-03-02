#ifndef VERIFYTAPN_TAPN_TIMEDINPUTARC_HPP_
#define VERIFYTAPN_TAPN_TIMEDINPUTARC_HPP_

#include <vector>
#include "TimeInterval.hpp"
#include "boost/smart_ptr.hpp"

namespace VerifyTAPN {
	namespace TAPN {
		class TimedTransition;
		class TimedPlace;

		class TimedInputArc
		{
		public: // typedefs
			typedef std::vector< boost::shared_ptr<TimedInputArc> > Vector;
			typedef std::vector< boost::weak_ptr<TimedInputArc> > WeakPtrVector;
		public:
			TimedInputArc(const boost::shared_ptr<TimedPlace>& place, const boost::shared_ptr<TimedTransition>& transition) : interval(), place(place), transition(transition) { };
			TimedInputArc(const boost::shared_ptr<TimedPlace>& place, const boost::shared_ptr<TimedTransition>& transition, const TimeInterval& interval) : interval(interval), place(place), transition(transition) { };
			virtual ~TimedInputArc() { /* empty */}

		public: // modifiers
			inline TimedPlace& InputPlace() { return *place; }
			inline TimedTransition& OutputTransition() { return *transition; }
			inline const TimeInterval& Interval() { return interval; }

		public: // Inspectors
			void Print(std::ostream& out) const;
		private:
			const TimeInterval interval;
			const boost::shared_ptr<TimedPlace> place;
			const boost::shared_ptr<TimedTransition> transition;
		};

		inline std::ostream& operator<<(std::ostream& out, const TimedInputArc& arc)
		{
			arc.Print(out);
			return out;
		}
	}
}

#endif /* VERIFYTAPN_TAPN_TIMEDINPUTARC_HPP_ */
