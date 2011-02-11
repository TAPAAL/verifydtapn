#include "DBMMarking.hpp"

namespace VerifyTAPN
{
	// Add a token in each output place of placesOfTokensToAdd
	// and add placesOfTokensToAdd.size() clocks to the DBM.
	// The DBM library requires arrays of bitvectors indicating
	// which tokens are in the original dbm (bitSrc) and which are in the resulting DBM (bitDst).
	void DBMMarking::AddTokens(const std::list<int>& placeIndices)
	{
		unsigned int nAdditionalTokens = placeIndices.size();
		unsigned int oldDimension = dbm.getDimension();
		unsigned int newDimension = oldDimension + nAdditionalTokens;

		unsigned int bitArraySize = (newDimension % 32 == 0 ? newDimension/32 : newDimension/32+1);

		unsigned int bitSrc[bitArraySize];
		unsigned int bitDst[bitArraySize];
		unsigned int table[newDimension];

		// setup the bitvectors
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

		unsigned int i = 0;
		for(std::list<int>::const_iterator iter = placeIndices.begin(); iter != placeIndices.end(); ++iter)
		{
			dbm(oldDimension+i) = 0; // reset new clocks to zero
			mapping.AddTokenToMapping(oldDimension+i);
			dp.AddTokenInPlace(*iter);
			i++;
		}
	}

	// Remove each token in tokensToRemove from the placement vector and from the DBM.
	// These tokens represent tokens that are moving to BOTTOM.
	// The DBM library requires arrays of bitvectors indicating which tokens are in
	// the original dbm (bitSrc) and which are in the resulting DBM (bitDst).
	void DBMMarking::RemoveTokens(const std::vector<int>& tokenIndices)
	{
		std::vector<int> dbmTokensToRemove;
		for(unsigned int i = 0; i < tokenIndices.size(); ++i)
		{
			dbmTokensToRemove.push_back(mapping.GetMapping(tokenIndices[i]));
		}

		unsigned int oldDimension = dbm.getDimension();

		assert(oldDimension-dbmTokensToRemove.size() > 0); // should at least be the zero clock left in the DBM

		unsigned int bitArraySize = (oldDimension % 32 == 0 ? oldDimension/32 : oldDimension/32+1);

		unsigned int bitSrc[bitArraySize];
		unsigned int bitDstMask[bitArraySize];
		unsigned int bitDst[bitArraySize];
		unsigned int table[oldDimension];

		// setup the bitvectors
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
		for(int i = tokenIndices.size()-1; i >= 0; --i)
		{
			mapping.RemoveToken(tokenIndices[i]);
			dp.RemoveToken(tokenIndices[i]);
		}
	}

	void DBMMarking::InitMapping()
	{
		std::vector<int> pVector = dp.GetTokenPlacementVector();
		std::vector<unsigned int> map;
		int i = 0;

		for(std::vector<int>::const_iterator iter = pVector.begin(); iter != pVector.end(); ++iter)
		{
			map.push_back(i+1);
			i++;
		}

		mapping = TokenMapping(map);
	}

	relation DBMMarking::ConvertToRelation(relation_t relation) const
	{
		switch(relation)
		{
		case base_SUPERSET: return SUPERSET;
		case base_SUBSET: return SUBSET;
		case base_EQUAL: return EQUAL;
		default: return DIFFERENT;
		}
	}

//		// Sort the state internally to obtain a canonical form.
//		// Used for symmetry reduction: if two states are symmetric they will have the same canonical form.
//		// The placement vector is sorted in ascending order, tokens in the same place are sorted on their lower bound,
//		// subsequently on their upper bound and finally by diagonal constraints if necessary.
//		void SymMarking::Canonicalize()
//		{
//			quickSort(0, dp.size()-1);
//		}
//
//		void SymMarking::BubbleSort()
//		{
//			for(unsigned int i = 0; i < dp.size(); ++i)
//			{
//				for(unsigned int j = dp.size()-1; j > i; --j)
//				{
//					if(TokenIGreaterThanTokenJ(j-1,j))
//					{
//						Swap(j-1,j);
//					}
//				}
//			}
//		}
//
//		bool SymMarking::TokenIGreaterThanTokenJ(int i, int j)
//		{
//			int placeI = dp.GetTokenPlacement(i);
//			int placeJ = dp.GetTokenPlacement(j);
//			unsigned int mapI = mapping.GetMapping(i);
//			unsigned int mapJ = mapping.GetMapping(j);
//
//			return placeI > placeJ
//					|| (placeI == placeJ && dbm(0,mapI) >  dbm(0,mapJ))
//					|| (placeI == placeJ && dbm(0,mapI) == dbm(0,mapJ) && dbm(mapI,0) > dbm(mapJ,0))
//					|| (placeI == placeJ && dbm(0,mapI) == dbm(0,mapJ) && dbm(mapI,0) == dbm(mapJ,0) && dbm(mapJ,mapI) > dbm(mapI,mapJ));
//		}
//
//		void SymMarking::quickSort(int left, int right)
//		{
//			if(right > left)
//			{
//				int pivot = left + (right - left)/2;
//				int newPivot = Partition(left, right, pivot);
//				quickSort(left, newPivot - 1);
//				quickSort(newPivot + 1, right);
//			}
//		}
//
//		int SymMarking::Partition(int left, int right, int pivot)
//		{
//			Swap(pivot, right);
//			int indexToReturn = left;
//			for(int i = left; i < right; ++i)
//			{
//				//if(IsLowerPositionLessThanPivot(i, right)) // Does not give optimal stored states unless its changed to mapPivot > mapLower in function
//				if(!IsUpperPositionGreaterThanPivot(i, right))
//				{
//					Swap(i, indexToReturn);
//					indexToReturn++;
//				}
//			}
//			Swap(indexToReturn, right);
//			return indexToReturn;
//		}
//
//		bool SymMarking::IsLowerPositionLessThanPivot(int lower, int pivotIndex) const
//		{
//			int placeLower = dp.GetTokenPlacement(lower);
//			int pivot = dp.GetTokenPlacement(pivotIndex);
//			unsigned int mapLower = mapping.GetMapping(lower);
//			unsigned int mapPivot = mapping.GetMapping(pivotIndex);
//
//			return placeLower < pivot
//					|| (placeLower == pivot && dbm(0,mapLower) <  dbm(0,mapPivot))
//					|| (placeLower == pivot && dbm(0,mapLower) == dbm(0,mapPivot) && dbm(mapLower,0) < dbm(mapPivot,0))
//					|| (placeLower == pivot && dbm(0,mapLower) == dbm(0,mapPivot) && dbm(mapLower,0) == dbm(mapPivot,0) && (mapPivot < mapLower ? dbm(mapPivot,mapLower) < dbm(mapLower,mapPivot) : dbm(mapLower,mapPivot) < dbm(mapPivot,mapLower)));
//		}
//
//		bool SymMarking::IsUpperPositionGreaterThanPivot(int upper, int pivotIndex) const
//		{
//			int placeUpper = dp.GetTokenPlacement(upper);
//			int pivot = dp.GetTokenPlacement(pivotIndex);
//			unsigned int mapUpper = mapping.GetMapping(upper);
//			unsigned int mapPivot = mapping.GetMapping(pivotIndex);
//
//			return placeUpper > pivot
//					|| (placeUpper == pivot && dbm(0,mapUpper) >  dbm(0,mapPivot))
//					|| (placeUpper == pivot && dbm(0,mapUpper) == dbm(0,mapPivot) && dbm(mapUpper,0) > dbm(mapPivot,0))
//					|| (placeUpper == pivot && dbm(0,mapUpper) == dbm(0,mapPivot) && dbm(mapUpper,0) == dbm(mapPivot,0) && (mapPivot > mapUpper ? dbm(mapPivot,mapUpper) > dbm(mapUpper,mapPivot) : dbm(mapUpper,mapPivot) > dbm(mapPivot,mapUpper)));
//		}
//
//		void SymMarking::Swap(int i, int j)
//		{
//
//			dp.Swap(i,j);
//			dbm.swapClocks(mapping.GetMapping(i), mapping.GetMapping(j));
//		}
}
