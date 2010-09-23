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
			static TimeInvariant createFor(const std::string& invariant);
			void Print(std::ostream& out) const;

		private: // data
			bool strictComparison;
			int bound;
		};

	}

}

#endif /* VERIFYTAPN_TAPN_TIMEINVARIANT_HPP_ */
