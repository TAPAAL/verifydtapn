
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
		public: // static
			static const TimedPlace& Bottom() {
				static TimedPlace bottom;
				return bottom;
			}

			static const int BottomIndex() {
				return -1;
			}

			static const std::string BOTTOM_NAME;

		public: // typedefs
			typedef std::vector< boost::shared_ptr<TimedPlace> > Vector;

		public: // construction / destruction
			TimedPlace(const std::string& name, const std::string& id, const TimeInvariant timeInvariant)
			: name(name), id(id), timeInvariant(timeInvariant), index(-2), isUntimed(false) { };
			TimedPlace() : name(BOTTOM_NAME), timeInvariant(), index(BottomIndex()), isUntimed(false) { };
			virtual ~TimedPlace() { /* empty */ };

		public: // modifiers
			inline void MarkPlaceAsUntimed() { isUntimed = true; }
			inline void SetIndex(int i) { index = i; };
			inline void SetMaxConstant(int max) { maxConstant = max; }
		public: // inspection
			const std::string& GetName() const;
			const std::string& GetId() const;
			void Print(std::ostream& out) const;
			inline int GetIndex() const { return index; };
			inline const bool IsUntimed() const { return isUntimed; }
			inline const int GetMaxConstant() const { return maxConstant; }
			inline const TAPN::TimeInvariant& GetInvariant() const { return timeInvariant; };
		private: // data
			std::string	name;
			std::string id;
			TimeInvariant timeInvariant;
			int index;
			bool isUntimed;
			int maxConstant;
		};

		inline std::ostream& operator<<(std::ostream& out, const TimedPlace& place)
		{
			place.Print(out);
			return out;
		}

		// TAPAAL does not allow multiple places with the same name,
		// thus it is enough to use the name to determine equality.
		inline bool operator==(TimedPlace const& a, TimedPlace const& b)
		{
			return a.GetName() == b.GetName();
		}

		inline bool operator!=(TimedPlace const& a, TimedPlace const& b)
		{
			return !(a.GetName() == b.GetName());
		}
	}
}
#endif /* VERIFYYAPN_TAPN_TIMEDPLACE_HPP_ */
