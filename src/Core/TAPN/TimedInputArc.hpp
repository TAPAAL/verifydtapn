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
			TimedInputArc(const boost::shared_ptr<TimedPlace>& place, const boost::shared_ptr<TimedTransition>& transition, const int weight) : interval(), place(place), transition(transition), weight(weight) { };
			TimedInputArc(const boost::shared_ptr<TimedPlace>& place, const boost::shared_ptr<TimedTransition>& transition, const int weight, const TimeInterval& interval) : interval(interval), place(place), transition(transition), weight(weight) { };
			virtual ~TimedInputArc() { /* empty */}

		public: // modifiers
			inline TimedPlace& getInputPlace() { return *place; }
			inline TimedTransition& getOutputTransition() { return *transition; }
			inline const TimeInterval& getInterval() { return interval; }

		public: // Inspectors
			void print(std::ostream& out) const;
			inline const int getWeight() const { return weight; }
		private:
			const TimeInterval interval;
			const boost::shared_ptr<TimedPlace> place;
			const boost::shared_ptr<TimedTransition> transition;
			const int weight;
		};

		inline std::ostream& operator<<(std::ostream& out, const TimedInputArc& arc)
		{
			arc.print(out);
			return out;
		}
	}
}

#endif /* VERIFYTAPN_TAPN_TIMEDINPUTARC_HPP_ */
