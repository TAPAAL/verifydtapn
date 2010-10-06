#ifndef SYMMARKING_HPP_
#define SYMMARKING_HPP_

#include "DiscretePart.hpp"
#include "dbm/fed.h"

namespace VerifyTAPN {

// Class representing a symbolic marking.
class SymMarking {
public:
	public:// construction
		SymMarking(const std::vector<int>& placement) : dp(placement), dbm(placement.size()) { };
		SymMarking(const DiscretePart& dp, const dbm::dbm_t& dbm) : dp(dp), dbm(dbm) { };
		virtual ~SymMarking() { };


	public: // inspectors
		const dbm::dbm_t& GetZone() const;
	public: // modifiers
		const DiscretePart* GetDiscretePart() const;

	private: // data
		DiscretePart dp;
		dbm::dbm_t dbm;
};

}

#endif /* SYMMARKING_HPP_ */
