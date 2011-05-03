#ifndef DISCRETEINCLUSIONMARKINGFACTORY_HPP_
#define DISCRETEINCLUSIONMARKINGFACTORY_HPP_

#include "UppaalDBMMarkingFactory.hpp"
#include "DiscretePartInclusionMarking.hpp"

#include "dbm/print.h"
#include <iostream>

namespace VerifyTAPN {

class DiscreteInclusionMarkingFactory : public UppaalDBMMarkingFactory {
public:
	DiscreteInclusionMarkingFactory(const boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn) : UppaalDBMMarkingFactory(tapn), tapn(tapn) {};
	virtual ~DiscreteInclusionMarkingFactory() {};

	virtual StoredMarking* Convert(SymbolicMarking* marking) const // TODO: who should clean up marking?
	{
		DBMMarking* dbmMarking = static_cast<DBMMarking*>(marking);
		std::vector<int> eq;
		std::vector<int> inc(tapn->NumberOfPlaces(), 0);
		TokenMapping mapping;

		for(unsigned int i = 0; i < marking->NumberOfTokens(); i++)
		{
			int place = marking->GetTokenPlacement(i);
			if(BelongsToINC(i, *dbmMarking))
				inc[place]++;
			else
			{
				int newIndex = eq.size();
				eq.push_back(place);
				mapping.SetMapping(newIndex, marking->GetClockIndex(i));
			}
		}
		dbm::dbm_t dbm = projectToEQPart(eq, mapping, dbmMarking->GetDBM());

		return new DiscretePartInclusionMarking(dbmMarking->id, eq, inc, mapping, dbm);
	};

	virtual SymbolicMarking* Convert(StoredMarking* marking) const
	{
		DiscretePartInclusionMarking* dpiMarking = static_cast<DiscretePartInclusionMarking*>(marking);

		unsigned int tokens = dpiMarking->size();
		TokenMapping mapping;
		std::vector<int> dpVec;

		unsigned int nextIncIndex = dpiMarking->eq.size()+1;

		unsigned int i = 0, place_j = 0;
		while(i < dpiMarking->eq.size() || place_j < dpiMarking->inc.size())
		{
			unsigned int place_i = i < dpiMarking->eq.size() ? dpiMarking->eq[i] : std::numeric_limits<int>::max();

			if(i < dpiMarking->eq.size() && place_i <= place_j)
			{
				int newIndex = dpVec.size();
				dpVec.push_back(place_i);
				mapping.SetMapping(newIndex, dpiMarking->GetClockIndex(i));
				i++;
				continue;
			}
			else if(place_j < place_i && dpiMarking->inc[place_j] > 0)
			{
				for(int t = 0; t < dpiMarking->inc[place_j]; t++)
				{
					int newIndex = dpVec.size();
					dpVec.push_back(place_j);
					mapping.SetMapping(newIndex, nextIncIndex);
					assert(mapping.GetMapping(newIndex) == nextIncIndex);
					nextIncIndex++;
				}
			}
			place_j++;
		}

		for(unsigned int i = 0; i < dpVec.size(); i++)
		{
			assert(mapping.GetMapping(i) <= tokens+1);
		}


		DiscretePart dp(dpVec);
		dbm::dbm_t dbm = ProjectToAllClocks(dp, mapping, dpiMarking->dbm);
		assert(dp.size()+1 == dbm.getDimension());
		DBMMarking* result =  new DBMMarking(dp, mapping, dbm);
		result->id = dpiMarking->id;
		return result;
	};

	virtual void Release(SymbolicMarking* marking)
	{
		if(marking != 0) delete marking;
	};

	virtual void Release(StoredMarking* marking)
	{
		if(marking != 0) delete marking;
	};

private:
	bool BelongsToINC(int token, const DBMMarking& marking) const
	{
		int placeIndex = marking.GetTokenPlacement(token);
		const TimedPlace& place = tapn->GetPlace(placeIndex);

		assert(placeIndex != TAPN::TimedPlace::BottomIndex());
		if(place.GetInvariant() != TAPN::TimeInvariant::LS_INF) return false;
		if(place.HasInhibitorArcs()) return false;
		if(place.IsUntimed()) return true;

		raw_t lowerBound = marking.GetLowerBound(marking.GetClockIndex(token));
		if(lowerBound == dbm_bound2raw(-place.GetMaxConstant(), dbm_STRICT)) return true; // TODO: check that == is correct
		return false;
	};

	dbm::dbm_t projectToEQPart(const std::vector<int>& eq, TokenMapping& mapping, const dbm::dbm_t& dbm) const
	{
		unsigned int dim = dbm.getDimension();
		unsigned int bitArraySize = (dim % 32 == 0 ? dim/32 : dim/32+1);
		unsigned int bitSrc[bitArraySize];
		unsigned int bitDst[bitArraySize];
		unsigned int table[dim];

		for(unsigned int i = 0; i < bitArraySize; i++)
		{
			if(i == 0){
				bitSrc[i] = 1;
				bitDst[i] = 1;
			}else{
				bitSrc[i] = 0;
				bitDst[i] = 0;
			}
		}

		for(unsigned int i = 0; i < dim; i++)
		{
			table[i] = std::numeric_limits<unsigned int>::max();
			bitSrc[i/32] |= (1 << (i % 32));
		}

		for(unsigned int i = 0; i < eq.size(); i++)
		{
			unsigned int arrayIndex = mapping.GetMapping(i)/32;
			unsigned int offset = mapping.GetMapping(i) % 32;
			bitDst[arrayIndex] |= (1 << offset);
		}

		dbm::dbm_t copy(dbm);
		copy.resize(bitSrc, bitDst, bitArraySize, table);

		assert(dbm.getDimension() == dim);
		assert(eq.size()+1 == copy.getDimension());

		for(unsigned int i = 0; i < dim; ++i)
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

		return copy;
	};

	dbm::dbm_t ProjectToAllClocks(const DiscretePart& dp, const TokenMapping& mapping, const dbm::dbm_t& dbm) const
	{
		//std::cout << "eq-dbm: \n" << dbm << std::endl;
		unsigned int dim = dbm.getDimension();
		unsigned int totalClocks = dp.size()+1;
		if(dim == totalClocks) return dbm::dbm_t(dbm);

		unsigned int bitArraySize = (totalClocks % 32 == 0 ? totalClocks/32 : totalClocks/32+1);
		unsigned int bitSrc[bitArraySize];
		unsigned int bitDst[bitArraySize];
		unsigned int table[totalClocks];

		for(unsigned int i = 0; i < bitArraySize; ++i)
		{
			if(dim >= i*32 && dim < (i+1)*32)
				bitSrc[i] = 0 | ((1 << dim%32)-1);
			else if(dim >= i*32 && dim >= (i+1)*32)
				bitSrc[i] = ~(bitSrc[i] & 0);
			else
				bitSrc[i] = 0;

			bitDst[i] = ~(bitDst[i] & 0);
		}

		if(totalClocks % 32 != 0)
		{
			bitDst[bitArraySize-1] ^= ~((1 << totalClocks % 32)-1);
		}


		dbm::dbm_t copy(dbm);
		copy.resize(bitSrc, bitDst, bitArraySize, table);

		assert(dbm.getDimension() == dim);
		for(unsigned int i = 0; i < dp.size(); i++)
		{
			if(mapping.GetMapping(i) >= dim)
			{
				const TimedPlace& place = tapn->GetPlace(dp.GetTokenPlacement(i));
				copy.constrain(0, mapping.GetMapping(i), dbm_bound2raw(-place.GetMaxConstant(), dbm_STRICT));
			}
		}
		//std::cout << "computed-dbm: \n" << copy << std::endl;
		return copy;
	};
private:
	boost::shared_ptr<TAPN::TimedArcPetriNet> tapn;
};

}

#endif /* DISCRETEINCLUSIONMARKINGFACTORY_HPP_ */
