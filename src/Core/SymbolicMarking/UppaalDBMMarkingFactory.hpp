#ifndef UPPAALDBMMARKINGFACTORY_HPP_
#define UPPAALDBMMARKINGFACTORY_HPP_

#include "MarkingFactory.hpp"
#include "DBMMarking.hpp"

namespace VerifyTAPN {

	class UppaalDBMMarkingFactory : public MarkingFactory {
	protected:
		static id_type nextId;
	public:
		UppaalDBMMarkingFactory(const boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn)
		{
			DBMMarking::tapn = tapn;
		};
		virtual ~UppaalDBMMarkingFactory() {};

		virtual SymbolicMarking* InitialMarking(const DiscretePart& dp) const
		{
			dbm::dbm_t dbm(dp.size()+1);
			dbm.setZero();
			DBMMarking* marking = new DBMMarking(dp, dbm);
			marking->id = nextId++;
			return marking;
		};

		virtual SymbolicMarking* Clone(const SymbolicMarking& marking) const
		{
			DBMMarking* clone = new DBMMarking(static_cast<const DBMMarking&>(marking));
			clone->id = nextId++;
			return clone;
		};
		virtual StoredMarking* Convert(SymbolicMarking* marking) const { return static_cast<DBMMarking*>(marking); };
		virtual SymbolicMarking* Convert(StoredMarking* marking) const { return static_cast<DBMMarking*>(marking); };

		virtual void Release(SymbolicMarking* marking)
		{

		};

		virtual void Release(StoredMarking* marking)
		{

		};
	};

}

#endif /* UPPAALDBMMARKINGFACTORY_HPP_ */
