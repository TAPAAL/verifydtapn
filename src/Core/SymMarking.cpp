#include "SymMarking.hpp"
#include "../TAPN/TimedArcPetriNet.hpp"
#include "Pairing.hpp"

namespace VerifyTAPN {

using namespace VerifyTAPN::TAPN;

	/////////////////////////////////////////////
	// Construction
	/////////////////////////////////////////////
	SymMarking::SymMarking(const DiscretePart& dp, const dbm::dbm_t& dbm) : dp(dp), dbm(dbm)
	{
		initMapping();
	}

	/////////////////////////////////////////////
	// Initializers
	/////////////////////////////////////////////
	void SymMarking::initMapping()
	{
		std::vector<int> pVector = dp.GetTokenPlacementVector();
		std::vector<int> map;
		int i = 0;
		map.push_back(-2); // token 0 in the dbm is always the 0 clock, thus it does not map to a token in the net.

		for(std::vector<int>::const_iterator iter = pVector.begin(); iter != pVector.end(); ++iter)
		{
			if((*iter) != -1) // if not BOTTOM
			{
				map.push_back(i);

			}
			i++;
		}

		mapping = map;
	}

	/////////////////////////////////////////////
	// Inspectors
	/////////////////////////////////////////////
	const DiscretePart* SymMarking::GetDiscretePart() const
	{
		return &dp;
	}

	const dbm::dbm_t& SymMarking::Zone() const
	{
		return dbm;
	}


	const TokenMapping& SymMarking::GetTokenMapping() const
	{
		return mapping;
	}

	void SymMarking::GenerateDiscreteTransitionSuccessors(const VerifyTAPN::TAPN::TimedArcPetriNet& tapn, std::vector<SymMarking*>& succ) const
	{
		// call successor generator
	}

	void SymMarking::Delay()
	{
		dbm.up();
	}

	void SymMarking::ResetClock(int clockIndex)
	{
		dbm(clockIndex) = 0;
	}

	void SymMarking::MoveToken(int tokenIndex, int newPlaceIndex)
	{
		dp.MoveToken(tokenIndex, newPlaceIndex);
	}

	void SymMarking::MoveFirstTokenAtBottomTo(int newPlaceIndex)
	{
		dp.MoveFirstTokenAtBottomTo(newPlaceIndex);
	}

	void SymMarking::AddActiveToken(int nAdditionalTokens)
	{
		int newDimension = dbm.getDimension()+nAdditionalTokens;
		dbm::dbm_t newDbm(newDimension);
		newDbm.setInit();

		unsigned int bitSrc = 0;
		unsigned int bitDst = 0;
		int bitval = 1;
		int dim = 0;
		unsigned int table[newDimension];
		while(dim < newDimension)
		{
			if(dim < dbm.getDimension())
				bitSrc |= bitval;

			bitDst |= bitval;
			bitval <<= 1;
			table[dim] = dim;
			dim++;
		}

		dbm_shrinkExpand(dbm.getDBM(), newDbm.getDBM(), dbm.getDimension(), &bitSrc, &bitDst, 1, table);

		dbm = newDbm;

		// TODO: update token mapping
	}

	void SymMarking::RemoveInactiveToken(int nTokensToRemove)
	{

	}

	void SymMarking::Constrain(const int tokenIndex, const TAPN::TimeInterval& ti)
	{
		dbm.constrain(0, tokenIndex, ti.LowerBoundToDBMRaw());
		dbm.constrain(tokenIndex, 0, ti.UpperBoundToDBMRaw());
	}
}

