#ifndef SYMMARKING_HPP_
#define SYMMARKING_HPP_

#include <vector>
#include <list>
#include "DiscreteMarking.hpp"
//#include "DiscretePart.hpp"
#include "TokenMapping.hpp"
#include "boost/smart_ptr.hpp"
#include "dbm/fed.h"

namespace VerifyTAPN {
	class VerificationOptions;
	class Successor;
	namespace TAPN {
		class TimedArcPetriNet;
		class TimedTransition;
		class TimeInterval;
	}

	// Class representing a symbolic marking.
	class SymMarking {
		public: // typedefs
			typedef std::vector<boost::shared_ptr<SymMarking> > Vector;
			typedef long long id_type;
			static id_type nextId;

		public: // construction
			SymMarking(const DiscretePart & dp, const dbm::dbm_t & dbm): dp(dp), dbm(dbm), stateId(nextId++) { initMapping(); };
			SymMarking(const SymMarking& marking) : dp(marking.dp), dbm(marking.dbm), mapping(marking.mapping), stateId(nextId++) { };
			virtual ~SymMarking() { };

			virtual SymMarking* Clone() const { return new SymMarking(*this); }

		public: // Inspectors
			inline const DiscretePart& GetDiscretePart() const { return dp; };
			inline const int GetTokenPlacement(int tokenIndex) const { return dp.GetTokenPlacement(tokenIndex); }
			inline const unsigned int GetNumberOfTokens() const { return dp.size(); }
			inline const dbm::dbm_t& Zone() const { return dbm; }
			void Print(std::ostream& out) const;
			virtual bool Satisfies(int tokenIndex, const TAPN::TimeInterval& ti) const;
			inline const unsigned int GetDBMIndex(const unsigned int tokenIndex) const { return mapping.GetMapping(tokenIndex); }

		public: // Modifiers
			virtual void Delay() { dbm.up(); };
			virtual bool IsEmpty() { return dbm.isEmpty(); };
			virtual void Extrapolate(const int* maxConstants) { dbm.extrapolateMaxBounds(maxConstants); };
			virtual void MoveToken(int tokenIndex, int newPlaceIndex);
			virtual void ResetClock(int tokenIndex) { dbm(mapping.GetMapping(tokenIndex)) = 0; }
			void AddTokens(const std::list<int>& outputPlacesOfTokensToAdd);
			void RemoveTokens(const std::vector<int>& tokensToRemove);
			virtual void Constrain(int tokenIndex, const TAPN::TimeInterval& ti);
			inline void DBMIntern() { dbm.intern(); }
			void Canonicalize();
			inline void SetInit() { dbm.setInit(); }
			inline id_type Id() const { return stateId; };
		private: // Initializers
			void initMapping();

		private: // helper functions
			void quickSort(int lo, int hi);
			int  Partition(int left, int right, int pivot);
			void Swap(int token1, int token2);
			bool ShouldSwap(int i, int j);
			bool IsLowerPositionLessThanPivot(int lower, int pivotIndex) const;
			bool IsUpperPositionGreaterThanPivot(int upper, int pivotIndex) const;
			void BubbleSort();
			bool TokenIGreaterThanTokenJ(int i, int j);

		public: // Helper functions
			void GenerateDiscreteTransitionSuccessors(const TAPN::TimedArcPetriNet& tapn, const VerificationOptions& options, std::vector<VerifyTAPN::Successor>& succ) const;

		private:
			DiscretePart dp;
			dbm::dbm_t dbm;
			TokenMapping mapping;
			id_type stateId;
	};


	inline std::ostream& operator<<(std::ostream& out, const VerifyTAPN::SymMarking& marking)
	{
		marking.Print( out );
		return out;
	}
}

#endif /* SYMMARKING_HPP_ */
