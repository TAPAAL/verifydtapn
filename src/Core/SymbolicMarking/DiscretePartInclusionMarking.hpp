#ifndef DISCRETEPARTINCLUSIONMARKING_HPP_
#define DISCRETEPARTINCLUSIONMARKING_HPP_

#include "StoredMarking.hpp"
#include "dbm/fed.h"
#include <vector>
#include "boost/functional/hash.hpp"
#include <algorithm>
#include "TokenMapping.hpp"
#include "../../typedefs.hpp"
#include <iosfwd>

namespace VerifyTAPN {

class DiscretePartInclusionMarking : public StoredMarking {
	friend class DiscreteInclusionMarkingFactory;
public:
	DiscretePartInclusionMarking(id_type id, const std::vector<int>& eq, const std::vector<int>& inc, const TokenMapping& mapping, const dbm::dbm_t& dbm) : eq(eq), inc(inc), mapping(mapping), dbm(dbm), id(id) { };
	DiscretePartInclusionMarking(const DiscretePartInclusionMarking& dm) : eq(dm.eq), inc(dm.inc), mapping(dm.mapping), dbm(dm.dbm), id(dm.id) { };
	virtual ~DiscretePartInclusionMarking() { };

	virtual size_t HashKey() const { return boost::hash_range(eq.begin(), eq.end()); };

	virtual relation Relation(const StoredMarking& stored) const
	{
		const DiscretePartInclusionMarking& other = static_cast<const DiscretePartInclusionMarking&>(stored);

		if(eq != other.eq) return DIFFERENT; // Not sure we need to check this, the hashing ensures that we get a list with the same eq part?

		unsigned int place = inc.size();
		bool checkSuperset = false;
		relation result = EQUAL;

		assert(inc.size() == other.inc.size());
		for(unsigned int i = 0; i < inc.size(); i++)
		{
			if(inc[i] != other.inc[i])
			{
				if(inc[i] > other.inc[i]) checkSuperset = true;
				place = i;
				break;
			}
		}

		if(place != inc.size())
		{
			if(checkSuperset)
			{
				for(unsigned int i = place+1; i < inc.size(); i++)
				{
					if(inc[i] < other.inc[i]) return DIFFERENT;
				}
				result = SUPERSET;
			}
			else
			{
				for(unsigned int i = place+1; i < inc.size(); i++)
				{
					if(inc[i] > other.inc[i]) return DIFFERENT;
				}
				result = SUBSET;
			}
		}

		relation dbm_rel = ConvertToRelation(dbm.relation(other.dbm));

		if(result == dbm_rel) return result;
		if(result == EQUAL) return dbm_rel;
		if(result == SUBSET && dbm_rel == EQUAL) return SUBSET;
		if(result == SUPERSET && dbm_rel == EQUAL) return SUPERSET;

		return DIFFERENT;
	}

	virtual void Extrapolate(const int* maxConstants) { dbm.diagonalExtrapolateMaxBounds(maxConstants); };

	unsigned int GetClockIndex(unsigned int index) { return mapping.GetMapping(index); };

	unsigned int size() const
	{
		int size = eq.size();
		for(unsigned int i = 0; i < inc.size(); i++)
		{
			size += inc[i];
		}

		return size;
	};

	virtual unsigned int UniqueId() const { return id; };
	virtual void Print(std::ostream& out) const;
private:
	relation ConvertToRelation(relation_t relation) const
	{
		switch(relation)
		{
		case base_SUPERSET: return SUPERSET;
		case base_SUBSET: return SUBSET;
		case base_EQUAL: return EQUAL;
		default: return DIFFERENT;
		}
	}

private:
	std::vector<int> eq;
	std::vector<int> inc;
	TokenMapping mapping;
	dbm::dbm_t dbm;
	id_type id;
};

}

#endif /* DISCRETEPARTINCLUSIONMARKING_HPP_ */
