#ifndef DISCRETEINCLUSIONMARKINGFACTORY_HPP_
#define DISCRETEINCLUSIONMARKINGFACTORY_HPP_

#include "UppaalDBMMarkingFactory.hpp"
#include "DiscretePartInclusionMarking.hpp"

#include "dbm/print.h"
#include <iostream>

namespace VerifyTAPN {

class DiscreteInclusionMarkingFactory : public UppaalDBMMarkingFactory {
public:
	DiscreteInclusionMarkingFactory(const boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn) : UppaalDBMMarkingFactory(), tapn(tapn) {};
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

		DiscretePartInclusionMarking* result = new DiscretePartInclusionMarking(dbmMarking->id, eq, inc, mapping, dbm);
		return result;
	};

	virtual SymbolicMarking* Convert(StoredMarking* marking) const
	{
		DiscretePartInclusionMarking* dpiMarking = static_cast<DiscretePartInclusionMarking*>(marking);

		unsigned int tokens = dpiMarking->size();
		TokenMapping mapping;
		std::vector<int> dpVec;

		int nextEqIndex = 1;
		int nextIncIndex = dpiMarking->eq.size()+1;

		unsigned int i = 0, place_j = 0;
		while(i < dpiMarking->eq.size() || place_j < dpiMarking->inc.size())
		{
			unsigned int place_i = i < dpiMarking->eq.size() ? dpiMarking->eq[i] : std::numeric_limits<int>::max();

			if(i < dpiMarking->eq.size() && place_i <= place_j)
			{
				int newIndex = dpVec.size();
				dpVec.push_back(place_i);
				mapping.SetMapping(newIndex, dpiMarking->GetClockIndex(i));
				i++; nextEqIndex++;
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

		assert(nextEqIndex == dpiMarking->eq.size()+1);
		for(int i = 0; i < dpVec.size(); i++)
		{
			assert(mapping.GetMapping(i) != -1);
			assert(mapping.GetMapping(i) <= tokens+1);
		}


		DiscretePart dp(dpVec);
		dbm::dbm_t dbm = ProjectToAllClocks(dp, mapping, dpiMarking->dbm);
		assert(dp.size()+1 == dbm.getDimension());
		DBMMarking* result =  new DBMMarking(dp, mapping, dbm);
		result->id = dpiMarking->id;
		return result;
	};

private:
	bool BelongsToINC(int token, const DBMMarking& marking) const
	{
		int placeIndex = marking.GetTokenPlacement(token);
		const TimedPlace& place = tapn->GetPlace(placeIndex);

		// TODO: check invariant
		// TODO: check inhibitor arcs
		// TODO: check transport arcs

		raw_t lowerBound = marking.GetLowerBound(marking.GetClockIndex(token));
		if(lowerBound == dbm_bound2raw(-place.GetMaxConstant(), dbm_STRICT)) return true; // TODO: check that == is correct
		return false;
	};

	dbm::dbm_t projectToEQPart(const std::vector<int>& eq, TokenMapping& mapping, const dbm::dbm_t& dbm) const
	{ // TODO: This should work for more than 32 clocks!
		unsigned int dim = dbm.getDimension();
		unsigned int bitSrc = (1 << dim)-1;
		unsigned int bitDst = 1; // clock 0 should always be there
		unsigned int table[dim];

		for(unsigned int i = 0; i < dim; i++)
		{
			table[i] = std::numeric_limits<unsigned int>::max();
		}

		for(unsigned int i = 0; i < eq.size(); i++)
		{
			bitDst |= (1 << mapping.GetMapping(i));
		}

		dbm::dbm_t copy(dbm);
		copy.resize(&bitSrc, &bitDst, 1, table);

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
		unsigned int totalTokens = dp.size();
		if(dim == totalTokens+1) return dbm::dbm_t(dbm);

		unsigned int bitSrc = (1 << dim)-1;
		unsigned int bitDst = (1 << (totalTokens+1))-1;
		unsigned int table[dim];

		dbm::dbm_t copy(dbm);
		copy.resize(&bitSrc, &bitDst, 1, table);

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
