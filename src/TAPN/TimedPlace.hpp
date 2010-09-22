
#ifndef VERIFYYAPN_TAPN_TIMEDPLACE_HPP_
#define VERIFYYAPN_TAPN_TIMEDPLACE_HPP_

#include <string>
#include <vector>
#include "TimeInvariant.hpp"
using std::string;

namespace VerifyTAPN{
	namespace TAPN{
		class TimedPlace {
		public: // typedefs
			typedef std::vector<TimedPlace> Vector; // Probably need to be pointers?

		public: // construction / destruction
			TimedPlace(const string& name);

			virtual ~TimedPlace() { /* empty */ };
		public: // inspection
			const string& getName() const;
		private: // data
			const TimeInvariant timeInvariant;
			const std::string	name;
		};
	}
}
#endif /* VERIFYYAPN_TAPN_TIMEDPLACE_HPP_ */
