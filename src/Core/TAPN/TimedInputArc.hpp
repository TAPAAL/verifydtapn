#ifndef VERIFYTAPN_TAPN_TIMEDINPUTARC_HPP_
#define VERIFYTAPN_TAPN_TIMEDINPUTARC_HPP_

#include <vector>
#include "TimeInterval.hpp"

namespace VerifyTAPN {
	namespace TAPN {
		class TimedTransition;
		class TimedPlace;

		class TimedInputArc
		{
		public: // typedefs
			typedef std::vector< TimedInputArc* > Vector;
		public:
			TimedInputArc(TimedPlace& place, TimedTransition& transition, const int weight) : interval(), place(place), transition(transition), weight(weight) { };
			TimedInputArc(TimedPlace& place, TimedTransition& transition, const int weight, TimeInterval interval) : interval(interval), place(place), transition(transition), weight(weight) { };
			virtual ~TimedInputArc() { /* empty */}

		public: // modifiers
			inline TimedPlace& getInputPlace() const { return place; }
			inline TimedTransition& getOutputTransition() const { return transition; }
			const inline TimeInterval& getInterval() const { return interval; }
                        inline void divideIntervalBy(int divider) { interval.divideBoundsBy(divider); };

		public: // Inspectors
			void print(std::ostream& out) const;
			inline const int getWeight() const { return weight; }
		private:
			TimeInterval interval;
			TimedPlace& place;
			TimedTransition& transition;
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
