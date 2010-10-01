#ifndef VERIFYTAPN_TAPN_OUTPUTARC_HPP_
#define VERIFYTAPN_TAPN_OUTPUTARC_HPP_

#include <vector>
#include "boost/smart_ptr.hpp"
#include "TimedPlace.hpp"
#include "TimedTransition.hpp"

namespace VerifyTAPN {
	namespace TAPN {

		class OutputArc
		{
		public: // typedefs
			typedef std::vector< boost::shared_ptr<OutputArc> > Vector;
		public:
			OutputArc(const TimedTransition& transition, const TimedPlace& place)
				: transition(transition), place(place) { };
			virtual ~OutputArc() { /* empty */ }

		public: // inspectors
			void Print(std::ostream& out) const;
		private:
			const TimedTransition& transition;
			const TimedPlace& place;
		};

		inline std::ostream& operator<<(std::ostream& out, const OutputArc& arc)
		{
			arc.Print(out);
			return out;
		}
	}
}

#endif /* VERIFYTAPN_TAPN_OUTPUTARC_HPP_ */
