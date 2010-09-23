#ifndef VERIFYTAPN_TAPN_TIMEINTERVAL_HPP_
#define VERIFYTAPN_TAPN_TIMEINTERVAL_HPP_

#include <limits>
#include <iostream>

namespace VerifyTAPN {
	namespace TAPN {

		class TimeInterval
		{
		public: // Construction
			TimeInterval() : leftStrict(false), lowerBound(0), upperBound(std::numeric_limits<int>::max()), rightStrict(true){ };
			TimeInterval(bool leftStrict, int lowerBound, int upperBound, bool rightStrict) : leftStrict(leftStrict), lowerBound(lowerBound), upperBound(upperBound), rightStrict(rightStrict) { };
			TimeInterval(const TimeInterval& ti) : leftStrict(ti.leftStrict), lowerBound(ti.lowerBound), upperBound(ti.upperBound), rightStrict(ti.rightStrict) {};
			TimeInterval& operator=(const TimeInterval& ti)
			{
				leftStrict = ti.leftStrict;
				lowerBound = ti.lowerBound;
				upperBound = ti.upperBound;
				rightStrict = ti.rightStrict;
				return *this;
			}

			virtual ~TimeInterval() { /* empty */ }

		public: // inspectors
			void Print(std::ostream& out) const;

		public: // statics
			static TimeInterval CreateFor(const std::string& interval);

		private: // data
			bool leftStrict;
			int lowerBound;
			int upperBound;
			bool rightStrict;
		};

		inline std::ostream& operator<<(std::ostream& out, const TimeInterval& interval)
		{
			interval.Print(out);
			return out;
		}
	}
}

#endif /* VERIFYTAPN_TAPN_TIMEINTERVAL_HPP_ */
