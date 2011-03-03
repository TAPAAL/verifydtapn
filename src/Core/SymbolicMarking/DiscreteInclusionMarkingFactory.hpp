#ifndef DISCRETEINCLUSIONMARKINGFACTORY_HPP_
#define DISCRETEINCLUSIONMARKINGFACTORY_HPP_

#include "MarkingFactory.hpp"
#include "DiscretePartInclusionMarking.hpp"

namespace VerifyTAPN {

class DiscreteInclusionMarkingFactory : public MarkingFactory {
public:
	DiscreteInclusionMarkingFactory() {};
	virtual ~DiscreteInclusionMarkingFactory() {};

	virtual SymbolicMarking* InitialMarking(const DiscretePart& dp) const
	{
		dbm::dbm_t dbm(dp.size()+1);
		dbm.setZero();
		DiscretePartInclusionMarking* marking = new DiscretePartInclusionMarking(dp, dbm);
		//marking->id = UppaalDBMMarkingFactory::nextId++;
		return marking;
	};

	virtual SymbolicMarking* Clone(const SymbolicMarking& marking) const
	{
		DiscretePartInclusionMarking* clone = new DiscretePartInclusionMarking(static_cast<const DiscretePartInclusionMarking&>(marking));
//		clone->id = nextId++;
		return clone;
	};
	virtual StoredMarking* Convert(SymbolicMarking* marking) const { return static_cast<DiscretePartInclusionMarking*>(marking); };
	virtual SymbolicMarking* Convert(StoredMarking* marking) const { return static_cast<DiscretePartInclusionMarking*>(marking); };
};

}

#endif /* DISCRETEINCLUSIONMARKINGFACTORY_HPP_ */
