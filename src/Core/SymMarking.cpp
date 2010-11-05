#include "SymMarking.hpp"
#include "../TAPN/TimedArcPetriNet.hpp"
#include "Pairing.hpp"
#include "dbm/print.h"
#include "../Verification/SuccessorGenerator.hpp"

namespace VerifyTAPN {

using namespace VerifyTAPN::TAPN;


	SymMarking::SymMarking(const DiscretePart& dp, const dbm::dbm_t& dbm) : dp(dp), dbm(dbm)
	{
		initMapping();
	}

	void SymMarking::initMapping()
	{
		std::vector<int> pVector = dp.GetTokenPlacementVector();
		std::vector<int> map;
		int i = 0;

		for(std::vector<int>::const_iterator iter = pVector.begin(); iter != pVector.end(); ++iter)
		{
			map.push_back(i+1);
			i++;
		}

		mapping = map;
	}


	void SymMarking::GenerateDiscreteTransitionSuccessors(const VerifyTAPN::TAPN::TimedArcPetriNet& tapn, unsigned int kbound, std::vector<SymMarking*>& succ) const
	{
		SuccessorGenerator succGen(tapn, kbound);
		succGen.GenerateDiscreteTransitionsSuccessors(this, succ);
	}

	void SymMarking::ResetClock(int tokenIndex)
	{
		dbm(mapping.GetMapping(tokenIndex)) = 0;
	}

	void SymMarking::MoveToken(int tokenIndex, int newPlaceIndex)
	{
		dp.MoveToken(tokenIndex, newPlaceIndex);
	}

	int SymMarking::MoveFirstTokenAtBottomTo(int newPlaceIndex)
	{
		return dp.MoveFirstTokenAtBottomTo(newPlaceIndex);
	}

	void SymMarking::AddTokens(const std::vector<int>& outputPlacesOfTokensToAdd)
	{
		unsigned int nAdditionalTokens = outputPlacesOfTokensToAdd.size();
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

		for(unsigned int i = 0; i < nAdditionalTokens; ++i)
		{
			dbm(oldDimension+i) = 0; // reset new clocks to zero
			mapping.AddTokenToMapping(oldDimension+i);
			dp.AddTokenInPlace(outputPlacesOfTokensToAdd[i]);
		}

	}

	void SymMarking::RemoveTokens(const std::vector<int>& tokensToRemove)
	{
		std::vector<int> dbmTokensToRemove;
		for(unsigned int i = 0; i < tokensToRemove.size(); ++i)
		{
			dbmTokensToRemove.push_back(mapping.GetMapping(tokensToRemove[i]));
		}

		unsigned int oldDimension = dbm.getDimension();
		unsigned int newDimension = oldDimension-dbmTokensToRemove.size();

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


		for(unsigned int i = 0; i < dbmTokensToRemove.size(); ++i)
		{
			bitDstMask[dbmTokensToRemove[i]/32] |= (1 << dbmTokensToRemove[i] % 32);
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
		for(unsigned int i = 0; i < oldDimension; ++i)
		{
			if(table[i] != std::numeric_limits<unsigned int>().max())
			{
				for(unsigned int j = 0; j < mapping.size(); ++j)
				{
					if(mapping.GetMapping(j) == i)
						mapping.SetMapping(j, table[i]);
				}
			}
		}

		// remove tokens from mapping and placement
		for(int i = tokensToRemove.size()-1; i >= 0; --i)
		{
			mapping.RemoveToken(tokensToRemove[i]);
			dp.RemoveToken(tokensToRemove[i]);
		}
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

	void SymMarking::Canonicalize()
	{

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

