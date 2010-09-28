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
			OutputArc();
			virtual ~OutputArc() { /* empty */ }

		public: // inspectors
			void Print(std::ostream& out) const;
		private:
			const TimedPlace& place;
			const TimedTransition& transition;
		};

		inline std::ostream& operator<<(std::ostream& out, const OutputArc& arc)
		{
			arc.Print(out);
			return out;
		}
	}
}

#endif /* VERIFYTAPN_TAPN_OUTPUTARC_HPP_ */
