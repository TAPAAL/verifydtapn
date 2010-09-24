#ifndef VERIFYTAPN_TAPN_OUTPUTARC_HPP_
#define VERIFYTAPN_TAPN_OUTPUTARC_HPP_

#include <vector>

namespace VerifyTAPN {
	namespace TAPN {

		class OutputArc
		{
		public: // typedefs
			typedef std::vector< boost::shared_ptr<OutputArc> > Vector;
		public:
			OutputArc();
			virtual ~OutputArc() { /* empty */ }
		private:
			const TimedPlace& place;
			const TimedTransition& transition;
		};
	}
}

#endif /* VERIFYTAPN_TAPN_OUTPUTARC_HPP_ */
