
#ifndef VERIFYYAPN_TAPN_TIMEDPLACE_HPP_
#define VERIFYYAPN_TAPN_TIMEDPLACE_HPP_

#include <string>
#include <vector>
#include <iostream>
#include "TimeInvariant.hpp"
#include "TimedInputArc.hpp"
#include "OutputArc.hpp"
#include "boost/shared_ptr.hpp"

namespace VerifyTAPN{
	namespace TAPN{
		class TimedPlace {
		public: // typedefs
			typedef std::vector< boost::shared_ptr<TimedPlace> > Vector;

		public: // construction / destruction
			TimedPlace(const std::string& name, const TimeInvariant timeInvariant) : name(name), timeInvariant(timeInvariant) { };

			virtual ~TimedPlace() { /* empty */ };

		public: // modifiers
			void AddToPreset(const boost::shared_ptr<OutputArc>& arc);
			void AddToPostset(const boost::shared_ptr<TimedInputArc>& arc);
		public: // inspection
			const std::string& GetName() const;
			void Print(std::ostream& out) const;
		private: // data
			const std::string	name;
			const TimeInvariant timeInvariant;
			TimedInputArc::Vector postset;
			OutputArc::Vector preset;
		};

		inline std::ostream& operator<<(std::ostream& out, const TimedPlace& place)
		{
			place.Print(out);
			return out;
		}
	}
}
#endif /* VERIFYYAPN_TAPN_TIMEDPLACE_HPP_ */
