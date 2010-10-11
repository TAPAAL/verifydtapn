#ifndef SYMMARKING_HPP_
#define SYMMARKING_HPP_

#include <vector>
#include "DiscretePart.hpp"
#include "TokenMapping.hpp"
#include "boost/smart_ptr.hpp"
#include "dbm/fed.h"

namespace VerifyTAPN {
	namespace TAPN {
		class TimedArcPetriNet;
		class TimedTransition;
	}

	// Class representing a symbolic marking.
	class SymMarking {
		public: // typedefs
			typedef std::vector<boost::shared_ptr<SymMarking> > Vector;

		public: // construction
			SymMarking(const DiscretePart & dp, const dbm::dbm_t & dbm);
			virtual ~SymMarking() { };

		public: // Inspectors
			const DiscretePart *GetDiscretePart() const;
			const dbm::dbm_t & GetZone() const;
			const TokenMapping& GetTokenMapping() const;
			void GenerateDiscreteTransitionSuccessors(const VerifyTAPN::TAPN::TimedArcPetriNet & tapn, std::vector<SymMarking*>& succ) const;

		public: // Modifiers
			void Delay();

		private: // Initializers
			void initMapping(const std::vector<int> & placement);

		private: // Helper functions
			void GenerateDiscreteTransitionSuccessorFor(const TAPN::TimedTransition& t, std::vector<SymMarking*>& succ) const;

		private:
			DiscretePart dp;
			dbm::dbm_t dbm;
			TokenMapping mapping;

	};

}

#endif /* SYMMARKING_HPP_ */
