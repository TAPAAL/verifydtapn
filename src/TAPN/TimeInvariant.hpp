#ifndef VERIFYTAPN_TAPN_TIMEINVARIANT_HPP_
#define VERIFYTAPN_TAPN_TIMEINVARIANT_HPP_

namespace VerifyTAPN {
	namespace TAPN {

		class TimeInvariant
		{
		public: // construction/destruction
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
