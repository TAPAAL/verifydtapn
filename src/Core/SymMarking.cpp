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

	void SymMarking::AddActiveTokensToDBM(unsigned int nAdditionalTokens)
	{
		unsigned int oldDimension = dbm.getDimension();
		unsigned int newDimension = oldDimension + nAdditionalTokens;

		unsigned int bitArraySize = (newDimension % 32 == 0 ? newDimension/32 : newDimension/32+1);

		unsigned int bitSrc[bitArraySize];
		unsigned int bitDst[bitArraySize];
		unsigned int table[newDimension];

		for(unsigned int i = 0; i < bitArraySize; ++i)
		{
			if(oldDimension >= i*32 && oldDimension < (i+1)*32)
				bitSrc[i] = 0 | ((1 << oldDimension%32)-1);
			else if(oldDimension >= i*32 && oldDimension >= (i+1)*32)
				bitSrc[i] = ~(bitSrc[i] & 0);
			else
				bitSrc[i] = 0;

			bitDst[i] = ~(bitDst[i] & 0);
		}

		if(newDimension%32 != 0)
		{
			bitDst[bitArraySize-1] ^= ~((1 << newDimension % 32)-1);
		}



		dbm.resize(bitSrc, bitDst, bitArraySize, table);

		for(int i = 0; i < nAdditionalTokens; ++i)
		{
			dbm(oldDimension+i) = 0; // reset new clocks to zero
		}
	}

	void SymMarking::RemoveInactiveTokensFromDBM(const std::vector<int>& tokensToRemove)
	{
		int oldDimension = dbm.getDimension();
		int newDimension = oldDimension-tokensToRemove.size();

		assert(newDimension > 0); // should at least be the zero clock left in the DBM

		unsigned int bitArraySize = (oldDimension % 32 == 0 ? oldDimension/32 : oldDimension/32+1);

		unsigned int bitSrc[bitArraySize];
		unsigned int bitDstMask[bitArraySize];
		unsigned int bitDst[bitArraySize];
		unsigned int table[oldDimension];

		// init to zero
		for(unsigned int i = 0; i < bitArraySize; ++i)
		{
			bitSrc[i] = ~(bitSrc[i] & 0);
			bitDst[i] = ~(bitDst[i] & 0);
			bitDstMask[i] = 0;
		}

		if(oldDimension%32 != 0)
		{
			bitSrc[bitArraySize-1] ^= ~((1 << oldDimension % 32)-1);
			bitDst[bitArraySize-1] ^= ~((1 << oldDimension % 32)-1);
		}


		for(unsigned int i = 0; i < tokensToRemove.size(); ++i)
		{
			bitDstMask[tokensToRemove[i]/32] |= (1 << tokensToRemove[i] % 32);
		}

		for(unsigned int i = 0; i < bitArraySize; ++i)
		{
			bitDst[i] ^= bitDstMask[i];
		}

		for(unsigned int i = 0; i < oldDimension; ++i)
		{
			table[i] = std::numeric_limits<unsigned int>().max();
		}

		dbm.resize(bitSrc,bitDst,bitArraySize, table);

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

