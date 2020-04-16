#ifndef VERIFYTAPN_TAPN_TIMEINTERVAL_HPP_
#define VERIFYTAPN_TAPN_TIMEINTERVAL_HPP_

#include <limits>
#include <iostream>
#include <algorithm>
#include <map>
#include <cassert>

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
                        void divideBoundsBy(int divider);
                        
                        
		public: // inspectors
			void print(std::ostream& out) const;
			inline const int getLowerBound() const { return lowerBound; }
			inline const int getUpperBound() const { return upperBound; }
			inline const bool isLowerBoundStrict() const { return leftStrict; }
			inline const bool isUpperBoundStrict() const { return rightStrict; }
                        inline bool setUpperBound(int bound, bool isStrict)
                        {
                            if(upperBound == bound) rightStrict |= isStrict;
                            else if(upperBound > bound){
                                rightStrict = isStrict;
                                upperBound = bound;
                            }
                            if(upperBound < lowerBound) return false;
                            else return true;
                        }

			inline const bool isZeroInfinity() const { return !leftStrict && lowerBound == 0 && upperBound == std::numeric_limits<int>::max() && rightStrict; }
                        inline const bool contains(int number) const
                        {
                            return number >= lowerBound && number <= upperBound;
                        }
                        inline bool intersects(const TimeInterval& other) const
                        {
                            assert(!leftStrict);
                            assert(!other.leftStrict);
                            assert(!rightStrict || upperBound == std::numeric_limits<int>().max());
                            assert(!other.rightStrict || other.upperBound == std::numeric_limits<int>().max());
                            if(other.lowerBound >= lowerBound &&
                               other.lowerBound <= upperBound) return true;
                            if(lowerBound >= other.lowerBound &&
                               lowerBound <= other.upperBound) return true;
                            return false;
                        }
                        
		public: // statics
			static TimeInterval createFor(const std::string& interval, const std::map<std::string, int>& replace);


		private: // data
			bool leftStrict;
			int lowerBound;
			int upperBound;
			bool rightStrict;
		};

		inline std::ostream& operator<<(std::ostream& out, const TimeInterval& interval)
		{
			interval.print(out);
			return out;
		}
	}
}

#endif /* VERIFYTAPN_TAPN_TIMEINTERVAL_HPP_ */
