#ifndef DISCRETEMARKING_HPP_
#define DISCRETEMARKING_HPP_

#include "SymbolicMarking.hpp"
#include "DiscretePart.hpp"

namespace VerifyTAPN {

	class DiscreteMarking : public SymbolicMarking {
	public:
		DiscreteMarking(const DiscretePart& dp) : dp(dp) { };
		DiscreteMarking(const DiscreteMarking& dm) : dp(dm.dp) { };
		virtual ~DiscreteMarking() { };

	public:
		virtual void MoveToken(int token, int targetPlace) { dp.MoveToken(token, targetPlace); };
		virtual unsigned int NumberOfTokens() const { return dp.size(); };
		virtual unsigned int NumberOfTokensInPlace(int placeIndex) const { return dp.NumberOfTokensInPlace(placeIndex); };
		virtual int GetTokenPlacement(int token) const { return dp.GetTokenPlacement(token); };
		virtual void AddTokens(const std::vector<int>& placeIndices)
		{
			for(std::vector<int>::const_iterator i = placeIndices.begin(); i != placeIndices.end(); i++)
			{
				dp.AddTokenInPlace(*i);
			}
		};

		virtual void RemoveTokens(const std::vector<int>& tokenIndices)
		{
			for(std::vector<int>::const_iterator i = tokenIndices.begin(); i != tokenIndices.end(); i++)
			{
				dp.RemoveToken(*i);
			}
		};

		// Sort the state internally to obtain a canonical form.
		// Used for symmetry reduction: if two states are symmetric they will have the same canonical form.
		// The placement vector is sorted in ascending order, tokens in the same place are sorted on their lower bound,
		// subsequently on their upper bound and finally by diagonal constraints if necessary.
		virtual void MakeSymmetric(BiMap& indirectionTable)
		{
			typedef boost::bimap<unsigned int, unsigned int>::value_type element;
			for(unsigned int i = 0; i < dp.size(); i++)
			{
				indirectionTable.insert( element(i,i) );
			}

			quickSort(0, dp.size()-1, indirectionTable);
		};

	private:
		void quickSort(int left, int right, BiMap& indirectionTable)
		{
			if(right > left)
			{
				int pivot = left + (right - left)/2;
				int newPivot = Partition(left, right, pivot, indirectionTable);
				quickSort(left, newPivot - 1, indirectionTable);
				quickSort(newPivot + 1, right, indirectionTable);
			}
		};

		int Partition(int left, int right, int pivot, BiMap& indirectionTable)
		{
			Swap(pivot, right);
			UpdateIndirectionTable(indirectionTable, pivot, right);
			int indexToReturn = left;
			for(int i = left; i < right; ++i)
			{
				//if(IsLowerPositionLessThanPivot(i, right)) // Does not give optimal stored states unless its changed to mapPivot > mapLower in function
				if(!IsUpperPositionGreaterThanPivot(i, right))
				{
					Swap(i, indexToReturn);
					UpdateIndirectionTable(indirectionTable, i, indexToReturn);
					indexToReturn++;
				}
			}
			Swap(indexToReturn, right);
			UpdateIndirectionTable(indirectionTable, indexToReturn, right);
			return indexToReturn;
		};

	protected:
		virtual bool IsUpperPositionGreaterThanPivot(int upper, int pivotIndex) const
		{
			int placeUpper = dp.GetTokenPlacement(upper);
			int pivot = dp.GetTokenPlacement(pivotIndex);
			return placeUpper > pivot;
		};

		virtual void Swap(int i, int j)
		{
			dp.Swap(i,j);
		};

		virtual void UpdateIndirectionTable(BiMap& indirectionTable, unsigned int i, unsigned int j)
		{
			if(i != j)
			{
				BiMap::right_iterator i_iterator = indirectionTable.right.find(i);
				BiMap::value_type new_i(i_iterator->second, j);
				indirectionTable.right.erase(i_iterator);

				BiMap::right_iterator j_iterator = indirectionTable.right.find(j);
				BiMap::value_type new_j(j_iterator->second, i);
				indirectionTable.right.erase(j_iterator);

				indirectionTable.insert(new_i);
				indirectionTable.insert(new_j);
			}
		}
	protected: // data
		DiscretePart dp;
	};
}

#endif /* DISCRETEMARKING_HPP_ */
