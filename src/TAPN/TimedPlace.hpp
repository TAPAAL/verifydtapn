
#ifndef VERIFYYAPN_TAPN_TIMEDPLACE_HPP_
#define VERIFYYAPN_TAPN_TIMEDPLACE_HPP_

#include <string>
#include <vector>
#include <iostream>
#include "TimeInvariant.hpp"
#include "boost/ptr_container/ptr_vector.hpp"

namespace VerifyTAPN{
	namespace TAPN{
		class TimedPlace {
		public: // typedefs
			typedef boost::ptr_vector<TimedPlace> Vector;
		public: // construction / destruction
			TimedPlace(const std::string& name, const TimeInvariant timeInvariant) : name(name), timeInvariant(timeInvariant) { };

			virtual ~TimedPlace() { /* empty */ };
		public: // inspection
			const std::string& GetName() const;
			void Print(std::ostream& out) const;
		private: // data
			const std::string	name;
			const TimeInvariant timeInvariant;
		};

		inline std::ostream& operator<<(std::ostream& out, const TimedPlace& place)
		{
			place.Print(out);
			return out;
		}
	}
}
#endif /* VERIFYYAPN_TAPN_TIMEDPLACE_HPP_ */
