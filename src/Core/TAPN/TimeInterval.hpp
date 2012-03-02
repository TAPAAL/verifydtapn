#ifndef VERIFYTAPN_TAPN_TIMEINTERVAL_HPP_
#define VERIFYTAPN_TAPN_TIMEINTERVAL_HPP_

#include <limits>
#include <iostream>
#include "dbm/constraints.h"

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
			inline const int GetLowerBound() const { return lowerBound; }
			inline const int GetUpperBound() const { return upperBound; }
			inline const bool IsLowerBoundStrict() const { return leftStrict; }
			inline const bool IsUpperBoundStrict() const { return rightStrict; }
			inline raw_t LowerBoundToDBMRaw() const
			{
				return dbm_bound2raw(-lowerBound, leftStrict ? dbm_STRICT : dbm_WEAK);
			};

			inline raw_t UpperBoundToDBMRaw() const
			{
				if(upperBound == std::numeric_limits<int>().max())
				{
					return dbm_LS_INFINITY;
				}
				else
				{
					return dbm_bound2raw(upperBound, rightStrict ? dbm_STRICT : dbm_WEAK);
				}
			};

			inline const bool IsZeroInfinity() const { return !leftStrict && lowerBound == 0 && upperBound == std::numeric_limits<int>().max() && rightStrict; }

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
