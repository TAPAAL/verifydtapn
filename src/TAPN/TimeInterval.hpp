#ifndef VERIFYTAPN_TAPN_TIMEINTERVAL_HPP_
#define VERIFYTAPN_TAPN_TIMEINTERVAL_HPP_

#include <limits>

namespace VerifyTAPN {
	namespace TAPN {

		class TimeInterval
		{
		public:
			TimeInterval() : leftStrict(false), lowerBound(0),
				upperBound(std::numeric_limits<int>::max()), rightStrict(true){ };
			TimeInterval(bool leftStrict, int lowerBound, int upperBound, bool rightStrict)
				: leftStrict(leftStrict), lowerBound(lowerBound),
				  upperBound(upperBound), rightStrict(rightStrict) { };

			virtual ~TimeInterval() { /* empty */ }

		private: // data
			const bool leftStrict;
			const int lowerBound;
			const int upperBound;
			const bool rightStrict;
		};
	}
}

#endif /* VERIFYTAPN_TAPN_TIMEINTERVAL_HPP_ */
