#include "SymMarking.hpp"
#include "../TAPN/TimedArcPetriNet.hpp"
#include "Pairing.hpp"
#include "dbm/print.h"

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

	int SymMarking::MoveFirstTokenAtBottomTo(int newPlaceIndex)
	{
		return dp.MoveFirstTokenAtBottomTo(newPlaceIndex);
	}

	void SymMarking::AddActiveTokensToDBM(int nAdditionalTokens)
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
	}

	void SymMarking::RemoveInactiveTokensFromDBM(const std::vector<int>& tokensToRemove)
	{
		int oldDimension = dbm.getDimension();
		int newDimension = oldDimension-tokensToRemove.size();


		assert(newDimension > 0); // should at least be the zero clock left in the DBM

		dbm::dbm_t newDbm(newDimension);
		newDbm.setInit();

		unsigned int bitSrc = 0;
		unsigned int bitDst = 0;
		int bitval = 1;
		unsigned int table[oldDimension];
		int dim = 0;

		while(dim < oldDimension)
		{
			if(dim == 0 || std::find(tokensToRemove.begin(), tokensToRemove.end(), dim) == tokensToRemove.end())
				bitDst |= bitval;

			bitSrc |= bitval;
			bitval <<= 1;
			table[dim] = std::numeric_limits<int>().max();
			dim++;
		}

		dbm_shrinkExpand(dbm.getDBM(), newDbm.getDBM(), dbm.getDimension(), &bitSrc, &bitDst, 1, table);
		dbm = newDbm;

		// fix token mapping according to new DBM:
		std::vector<int> newTokenMap;

		int j = 0;
		for(int i = 0; i < oldDimension; ++i)
		{
			if(table[i] != std::numeric_limits<int>().max())
				newTokenMap.push_back(mapping.GetMapping(i));
		}

		mapping = newTokenMap;
	}

	void SymMarking::Constrain(const int tokenIndex, const TAPN::TimeInterval& ti)
	{
		dbm.constrain(0, tokenIndex, ti.LowerBoundToDBMRaw());
		dbm.constrain(tokenIndex, 0, ti.UpperBoundToDBMRaw());
	}

	void SymMarking::AddTokenToMapping(int tokenIndex)
	{
		mapping.AddTokenToMapping(tokenIndex);
	}

	void SymMarking::Print(std::ostream& out) const
	{
		out << "Symbolic Marking:\n";
		out << "-------------------------\n";
		out << "Placement Vector:\n";

		int i = 0;
		for(std::vector<int>::const_iterator iter = dp.GetTokenPlacementVector().begin();iter != dp.GetTokenPlacementVector().end();++iter){
			out << i << ":" << (*iter) << "\n";
			i++;
		}
		out << "\n\nMapping Vector:\n";
		i = 0;
		for(std::vector<int>::const_iterator iter2 = mapping.GetMappingVector().begin();iter2 != mapping.GetMappingVector().end();++iter2){
			out << i << ":" << (*iter2) << "\n";
			i++;
		}
		out << "\nDBM:\n";
		out << dbm << "\n\n";
	}
}

