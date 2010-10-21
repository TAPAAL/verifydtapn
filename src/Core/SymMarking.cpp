#include "SymMarking.hpp"
#include "../TAPN/TimedArcPetriNet.hpp"
#include "Pairing.hpp"
#include "dbm/print.h"
#include "../Verification/SuccessorGenerator.hpp"

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

	void SymMarking::GenerateDiscreteTransitionSuccessors(const VerifyTAPN::TAPN::TimedArcPetriNet& tapn, unsigned int kbound, std::vector<SymMarking*>& succ) const
	{
		SuccessorGenerator succGen(tapn, kbound);
		succGen.GenerateDiscreteTransitionsSuccessors(this, succ);
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
		int oldDimension = dbm.getDimension();
		int newDimension = oldDimension + nAdditionalTokens;
		dbm::dbm_t newDbm(newDimension);
		newDbm.setInit();

		unsigned int bitArraySize = newDimension/32+1;

		unsigned int bitSrc[bitArraySize];
		unsigned int bitDst[bitArraySize];
		unsigned int table[newDimension];

		// init to zero
		for(int i = 0; i < bitArraySize; ++i)
		{
			bitSrc[i] = 0;
			bitDst[i] = 0;
		}

		int j = 0;

		while(oldDimension-j*32 > 32)
		{
			bitSrc[j] = ~(bitSrc[j] & 0);
			bitDst[j] = ~(bitDst[j] & 0);
			j++;
		}

		//set final bits of bitSrc
		for(int i = j; i < oldDimension; i++)
		{
			bitSrc[i/32] |= (1 << i % 32);
		}

		//set final bits of bitDst
		for(int i = j; i < newDimension; i++)
		{
			bitDst[i/32] |= (1 << i % 32);
			table[i] = std::numeric_limits<unsigned int>().max();
		}


		dbm_shrinkExpand(dbm.getDBM(), newDbm.getDBM(), oldDimension, bitSrc, bitDst, bitArraySize, table);

		for(int i = 0; i < nAdditionalTokens; ++i)
		{
			newDbm(oldDimension+i) = 0; // reset new clocks to zero
		}

		dbm = newDbm;
	}

	void SymMarking::RemoveInactiveTokensFromDBM(const std::vector<int>& tokensToRemove)
	{
		int oldDimension = dbm.getDimension();
		int newDimension = oldDimension-tokensToRemove.size();


		assert(newDimension > 0); // should at least be the zero clock left in the DBM

		dbm::dbm_t newDbm(newDimension);
		newDbm.setInit();

		unsigned int bitArraySize = oldDimension/32+1;

		unsigned int bitSrc[bitArraySize];
		unsigned int bitDst[bitArraySize];
		unsigned int table[oldDimension];

		// init to zero
		for(int i = 0; i < bitArraySize; ++i)
		{
			bitSrc[i] = 0;
			bitDst[i] = 0;
		}

		for(int i = 0; i < oldDimension; ++i)
		{
			table[i] = std::numeric_limits<unsigned int>().max();
		}

		int j = 0;

		while(oldDimension-j*32 > 32)
		{
			bitSrc[j] = ~(bitSrc[j] & 0);
			j++;
		}

		//set final bits of bitSrc
		for(int i = j; i < oldDimension; i++)
		{
			bitSrc[i/32] |= (1 << i % 32);

			if(i == 0 || std::find(tokensToRemove.begin(), tokensToRemove.end(), i) == tokensToRemove.end())
				bitDst[i/32] |= (1 << i % 32);
		}


		dbm_shrinkExpand(dbm.getDBM(), newDbm.getDBM(), oldDimension, bitSrc, bitDst, bitArraySize, table);
		dbm = newDbm;

		// fix token mapping according to new DBM:
		std::vector<int> newTokenMap;

		for(int i = 0; i < oldDimension; ++i)
		{
			if(table[i] != std::numeric_limits<unsigned int>().max())
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

