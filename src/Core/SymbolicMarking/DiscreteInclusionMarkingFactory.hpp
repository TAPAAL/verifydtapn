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
		TokenMapping mapping(marking->NumberOfTokens());

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

		return new DiscretePartInclusionMarking(eq, inc, resize(eq, mapping, dbmMarking->GetDBM()));
	};

	virtual SymbolicMarking* Convert(StoredMarking* marking) const
	{
		DiscretePartInclusionMarking* dpiMarking = static_cast<DiscretePartInclusionMarking*>(marking);

		unsigned int tokens = dpiMarking->size();
		TokenMapping mapping(tokens);
		std::vector<int> dpVec;

		int nextEqIndex = 1;
		int nextIncIndex = dpiMarking->eq.size()+1;

		unsigned int i = 0, place_j = 0;
		while(i < dpiMarking->eq.size() || place_j < dpiMarking->inc.size())
		{
			unsigned int place_i = dpiMarking->eq[i];

			if(i < dpiMarking->eq.size() && place_i <= place_j)
			{
				int newIndex = dpVec.size();
				dpVec.push_back(place_i);
				mapping.SetMapping(newIndex, nextEqIndex);
				assert(mapping.GetMapping(newIndex) == nextEqIndex);
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

		return new DBMMarking(dp, mapping, scaleUp(dp, mapping, dpiMarking->dbm));
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

	dbm::dbm_t resize(const std::vector<int>& eq, const TokenMapping& mapping, const dbm::dbm_t& dbm) const
	{ // TODO: This should work for more than 32 clocks!
		int dim = dbm.getDimension();
		unsigned int bitSrc = (1 << dim)-1;
		unsigned int bitDst = 1; // clock 0 should always be there
		unsigned int table[dim];

		for(unsigned int i = 0; i < eq.size(); i++)
		{
			bitDst |= (1 << mapping.GetMapping(i));
		}

		dbm::dbm_t copy(dbm);
		copy.resize(&bitSrc, &bitDst, 1, table);

		assert(dbm.getDimension() == dim);
		assert(eq.size()+1 == copy.getDimension());

		return copy;
	};

	dbm::dbm_t scaleUp(const DiscretePart& dp, const TokenMapping& mapping, const dbm::dbm_t& dbm) const
	{
		std::cout << "eq-dbm: \n" << dbm << std::endl;
		int dim = dbm.getDimension();
		int totalTokens = dp.size();
		if(dim == totalTokens+1) return dbm::dbm_t(dbm);

		unsigned int bitSrc = (1 << dim)-1;
		unsigned int bitDst = (1 << (totalTokens+1))-1;
		unsigned int table[dim];

		dbm::dbm_t copy(dbm);
		copy.resize(&bitSrc, &bitDst, 1, table);

		assert(dbm.getDimension() == dim);

		for(int i = 0; i < dp.size(); i++)
		{
			if(mapping.GetMapping(i) >= dim)
			{
				const TimedPlace& place = tapn->GetPlace(dp.GetTokenPlacement(i));
				copy.constrain(0,i, dbm_bound2raw(-place.GetMaxConstant(), dbm_STRICT));
			}
		}
		std::cout << "computed-dbm: \n" << copy << std::endl;
		return copy;
	};
private:
	boost::shared_ptr<TAPN::TimedArcPetriNet> tapn;
};

}

#endif /* DISCRETEINCLUSIONMARKINGFACTORY_HPP_ */
