#ifndef VERIFYTAPN_TAPN_TIMEINVARIANT_HPP_
#define VERIFYTAPN_TAPN_TIMEINVARIANT_HPP_

#include <limits>

namespace VerifyTAPN {
	namespace TAPN {

		class TimeInvariant
		{
		public: // construction/destruction
			TimeInvariant() : strictComparison(true), bound(std::numeric_limits<int>::max()) { };
			TimeInvariant(bool strictComparison, int bound) : strictComparison(strictComparison), bound(bound) { };

			virtual ~TimeInvariant() { /* Empty */ };
		public: // inspectors


		private: // data
			const bool strictComparison;
			const int bound;
		};

	}

}

#endif /* VERIFYTAPN_TAPN_TIMEINVARIANT_HPP_ */
