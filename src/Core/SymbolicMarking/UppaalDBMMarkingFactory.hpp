#ifndef UPPAALDBMMARKINGFACTORY_HPP_
#define UPPAALDBMMARKINGFACTORY_HPP_

#include "MarkingFactory.hpp"
#include "DBMMarking.hpp"

namespace VerifyTAPN {

	class UppaalDBMMarkingFactory : public MarkingFactory {
	public:
		UppaalDBMMarkingFactory() { };
		virtual ~UppaalDBMMarkingFactory() {};

		virtual SymbolicMarking* InitialMarking(const DiscretePart& dp) const
		{
			dbm::dbm_t dbm(dp.size()+1);
			dbm.setZero();
			return new DBMMarking(dp, dbm);
		};

		virtual StoredMarking* Convert(SymbolicMarking* marking) const { return static_cast<DBMMarking*>(marking); };
		virtual SymbolicMarking* Convert(StoredMarking* marking) const { return static_cast<DBMMarking*>(marking); };
	};

}

#endif /* UPPAALDBMMARKINGFACTORY_HPP_ */
