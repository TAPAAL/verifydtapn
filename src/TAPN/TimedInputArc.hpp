#ifndef VERIFYTAPN_TAPN_TIMEDINPUTARC_HPP_
#define VERIFYTAPN_TAPN_TIMEDINPUTARC_HPP_

#include "TimeInterval.hpp"

#include <vector>
#include <memory>

namespace VerifyTAPN {
	namespace TAPN {
		class TimedTransition;
		class TimedPlace;

		class TimedInputArc
		{
		public: // typedefs
			typedef std::vector< std::shared_ptr<TimedInputArc> > Vector;
			typedef std::vector< std::weak_ptr<TimedInputArc> > WeakPtrVector;
		public:
			TimedInputArc(const std::shared_ptr<TimedPlace>& place, const std::shared_ptr<TimedTransition>& transition) : interval(), place(place), transition(transition) { };
			TimedInputArc(const std::shared_ptr<TimedPlace>& place, const std::shared_ptr<TimedTransition>& transition, const TimeInterval& interval) : interval(interval), place(place), transition(transition) { };
			virtual ~TimedInputArc() { /* empty */}

		public: // modifiers
			inline TimedPlace& InputPlace() { return *place; }
			inline TimedTransition& OutputTransition() { return *transition; }
			inline const TimeInterval& Interval() { return interval; }

		public: // Inspectors
			void Print(std::ostream& out) const;
		private:
			const TimeInterval interval;
			const std::shared_ptr<TimedPlace> place;
			const std::shared_ptr<TimedTransition> transition;
		};

		inline std::ostream& operator<<(std::ostream& out, const TimedInputArc& arc)
		{
			arc.Print(out);
			return out;
		}
	}
}

#endif /* VERIFYTAPN_TAPN_TIMEDINPUTARC_HPP_ */
