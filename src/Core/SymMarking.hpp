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
		class TimeInterval;
	}

	// Class representing a symbolic marking.
	class SymMarking {
		public: // typedefs
			typedef std::vector<boost::shared_ptr<SymMarking> > Vector;

		public: // construction
			SymMarking(const DiscretePart & dp, const dbm::dbm_t & dbm);
			SymMarking(const SymMarking& marking) : dp(marking.dp), dbm(marking.dbm), mapping(marking.mapping) { };
			virtual ~SymMarking() { };

			SymMarking* clone() const { return new SymMarking(*this); }

		public: // Inspectors
			inline const DiscretePart& GetDiscretePart() const { return dp; };
			inline const int GetTokenPlacement(int tokenIndex) const { return dp.GetTokenPlacement(tokenIndex); }
			inline const unsigned int GetNumberOfTokens() const { return dp.size(); }
			inline const dbm::dbm_t& Zone() const { return dbm; }
			inline const TokenMapping& GetTokenMapping() const { return mapping; }
			void GenerateDiscreteTransitionSuccessors(const VerifyTAPN::TAPN::TimedArcPetriNet & tapn, std::vector<SymMarking*>& succ) const;
			void Print(std::ostream& out) const;

		public: // Modifiers
			inline void Delay() { dbm.up(); };
			inline void Extrapolate(const int* maxConstants) { dbm.extrapolateMaxBounds(maxConstants); };
			void MoveToken(int tokenIndex, int newPlaceIndex);
			int MoveFirstTokenAtBottomTo(int newPlaceIndex);
			void ResetClock(int tokenIndex);
			void AddTokens(const std::vector<int>& outputPlacesOfTokensToAdd);
			void RemoveTokens(const std::vector<int>& tokensToRemove);
			void Constrain(const int tokenIndex, const TAPN::TimeInterval& ti);
			void AddTokenToMapping(int tokenIndex);
			void MakeKBound(int kBound) { dp.MakeKBound(kBound); }
			inline void DBMIntern() { dbm.intern(); }
			void Canonicalize();

		private: // Initializers
			void initMapping();


		public: // Helper functions
			void GenerateDiscreteTransitionSuccessors(const TAPN::TimedArcPetriNet& tapn, unsigned int kbound, std::vector<SymMarking*>& succ) const;

		private:
			DiscretePart dp;
			dbm::dbm_t dbm;
			TokenMapping mapping;
	};


	inline std::ostream& operator<<(std::ostream& out, const VerifyTAPN::SymMarking& marking)
	{
		marking.Print( out );
		return out;
	}

}

#endif /* SYMMARKING_HPP_ */
