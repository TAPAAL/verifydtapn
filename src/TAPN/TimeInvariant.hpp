#ifndef VERIFYTAPN_TAPN_TIMEINVARIANT_HPP_
#define VERIFYTAPN_TAPN_TIMEINVARIANT_HPP_

#include <string>
#include <limits>

namespace VerifyTAPN {
	namespace TAPN {

		class TimeInvariant
		{
		public: // construction/destruction
			TimeInvariant() : strictComparison(true), bound(std::numeric_limits<int>::max()) { };
			TimeInvariant(bool strictComparison, int bound) : strictComparison(strictComparison), bound(bound) { };
			TimeInvariant(const TimeInvariant& ti) : strictComparison(ti.strictComparison), bound(ti.bound) { };
			TimeInvariant& operator=(const TimeInvariant& ti)
			{
				strictComparison = ti.strictComparison;
				bound = ti.bound;
				return *this;
			}

			virtual ~TimeInvariant() { /* Empty */ };
		public: // inspectors
			void Print(std::ostream& out) const;

		public: // statics
			static TimeInvariant CreateFor(const std::string& invariant);

		private: // data
			bool strictComparison;
			int bound;
		};

		inline std::ostream& operator<<(std::ostream& out, const TimeInvariant& invariant)
		{
			invariant.Print(out);
			return out;
		}
	}

}

#endif /* VERIFYTAPN_TAPN_TIMEINVARIANT_HPP_ */
