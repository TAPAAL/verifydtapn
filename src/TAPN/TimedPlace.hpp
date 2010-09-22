
#ifndef VERIFYYAPN_TAPN_TIMEDPLACE_HPP_
#define VERIFYYAPN_TAPN_TIMEDPLACE_HPP_

#include <string>
#include "TimeInvariant.hpp"
using std::string;

namespace VerifyTAPN{
	namespace TAPN{
		class TimedPlace {
		public: // construction / destruction
			TimedPlace(const string& name);

			virtual ~TimedPlace() { /* Destructors should be virtual and inline */ };
		public: // inspection
			const string& getName() const;
		private: // data
			const TimeInvariant timeInvariant;
			const std::string	name;
		};
	}
}
#endif /* VERIFYYAPN_TAPN_TIMEDPLACE_HPP_ */
