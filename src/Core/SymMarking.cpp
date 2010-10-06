#include "SymMarking.hpp"

namespace VerifyTAPN {
	const DiscretePart* SymMarking::GetDiscretePart() const
	{
		return &dp;
	}

	const dbm::dbm_t& SymMarking::GetZone() const
	{
		return dbm;
	}
}
