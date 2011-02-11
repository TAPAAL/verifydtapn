#ifndef UPPAALDBMMARKINGFACTORY_HPP_
#define UPPAALDBMMARKINGFACTORY_HPP_

#include "MarkingFactory.hpp"
#include "DBMMarking.hpp"

namespace VerifyTAPN {

	class UppaalDBMMarkingFactory : public MarkingFactory {
	private:
		static id_type nextId;
	public:
		UppaalDBMMarkingFactory() { };
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
	};

}

#endif /* UPPAALDBMMARKINGFACTORY_HPP_ */
