#ifndef DISCRETEPARTINCLUSIONMARKING_HPP_
#define DISCRETEPARTINCLUSIONMARKING_HPP_

#include "DBMMarking.hpp"
#include <map>
#include <iostream>
#include <algorithm>
#include "google/dense_hash_set"

namespace VerifyTAPN {

class DiscretePartInclusionMarking : public DBMMarking {
	struct Count{
		int one;
		int two;
	};

public:
	static TAPN::TimedArcPetriNet* tapn;
	static void set(TAPN::TimedArcPetriNet* tapn){ DiscretePartInclusionMarking::tapn = tapn; };

	DiscretePartInclusionMarking(const DiscretePart& dp, const dbm::dbm_t& dbm) : DBMMarking(dp, dbm) { };
	DiscretePartInclusionMarking(const DiscretePartInclusionMarking& dm) : DBMMarking(dm) { };
	virtual ~DiscretePartInclusionMarking() { };

	virtual size_t HashKey() const { return 0; }; // TODO: hash correctly

	virtual relation Relation(const StoredMarking& other) const
	{
		bool sameDP = eqdp()(this->dp, static_cast<const DiscretePartInclusionMarking&>(other).dp);
		if(sameDP)
			return DBMMarking::Relation(other);
		else
		{
			const DiscretePartInclusionMarking& b = static_cast<const DiscretePartInclusionMarking&>(other);
			typedef google::dense_hash_set<int> hashset;

			std::map<int, Count> counts;
			hashset incA;
			hashset incB;
			for(int i = 0; i < dp.size(); i++)
			{
				int placement = dp.GetTokenPlacement(i);
				if(dbm(0,i) >= dbm_bound2raw(-tapn->GetPlace(placement).GetMaxConstant(), dbm_STRICT)){
					incA.insert(i);
				}
				if(placement != TimedPlace::BottomIndex())
				{
					if(counts.count(placement) > 0)
					{
						Count& count = counts[placement];
						count.one++;
					}
					else
					{
						Count c;
						c.one = 1;
						c.two = 0;
						counts[placement] = c;
					}
				}
			}

			for(int i = 0; i < b.dp.size(); i++)
			{
				int placement = b.dp.GetTokenPlacement(i);
				if(b.dbm(0,i) >= dbm_bound2raw(-tapn->GetPlace(placement).GetMaxConstant(), dbm_STRICT)){
					incB.insert(i);
				}
				if(placement != TimedPlace::BottomIndex())
				{
					if(counts.count(placement) > 0)
					{
						Count& c = counts[placement];
						c.two++;
					}
					else
					{
						Count c;
						c.one = 0;
						c.two = 1;
						counts[placement] = c;
					}
				}
			}

			if(incA != incB) return DIFFERENT;

			for(std::map<int,Count>::iterator iter = counts.begin(); iter != counts.end(); iter++)
			{
				bool included = (*iter).second.one <= (*iter).second.two;
				if(!included){
					return DIFFERENT;
				}
			}

		//		if(strictly){
		//			std::cout << "*";
		//		}

			//return SUBSET;
			relation_t relation = resize(dp,mapping,dbm).relation(resize(b.dp, b.mapping, b.dbm));
			//relation_t relation = dbm.relation(b.dbm);
			return ConvertToRelation(relation);
		}
	}

	virtual void Extrapolate(const int* maxConstants) { dbm.diagonalExtrapolateMaxBounds(maxConstants); };
private:
	dbm::dbm_t resize(const DiscretePart& dp, const TokenMapping& mapping, const dbm::dbm_t& dbm) const
	{
		std::vector<int> inc;
		for(int i = 0; i < dp.size(); i++)
		{
			const TAPN::TimedPlace& place = tapn->GetPlace(dp.GetTokenPlacement(i));
			int maxConstant = place.GetMaxConstant();
			if(dbm(0,i) == dbm_bound2raw(-maxConstant, dbm_STRICT)){
				inc.push_back(i);
			}
		}
		int dim = dbm.getDimension();
		unsigned int bitSrc = 0;
		unsigned int bitDst = 0;
		unsigned int table[dim];

		bitSrc = bitDst = (1 << dim)-1;

		for(int i = 0; i < inc.size(); i++)
		{
			unsigned int mask = ~(1 << mapping.GetMapping(inc[i]));
			bitDst &= mask;
		}

		dbm::dbm_t copy(dbm);
		copy.resize(&bitSrc, &bitDst, 1, table);

		assert(dbm.getDimension() == dim);


		return copy;
	}
};

}

#endif /* DISCRETEPARTINCLUSIONMARKING_HPP_ */
