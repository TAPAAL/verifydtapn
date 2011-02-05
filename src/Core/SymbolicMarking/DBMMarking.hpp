#ifndef DBMMARKING_HPP_
#define DBMMARKING_HPP_

#include "DiscreteMarking.hpp"
#include "dbm/fed.h"

namespace VerifyTAPN {

	class DBMMarking: public DiscreteMarking {
	public:
		DBMMarking(const DiscretePart& dp, const dbm::dbm_t& dbm) : DiscreteMarking(dp), dbm(dbm) { };
		virtual ~DBMMarking() { };

		virtual DBMMarking* Clone() const { return NULL; };

		virtual void Reset(int token) { };
		virtual void Constrain(int token, const TAPN::TimeInterval& interval) { };
		virtual bool Satisfies(int token, const TAPN::TimeInterval& interval) const { return true; };
		virtual bool IsEmpty() const { return true; };
		virtual void Delay() { };
		virtual void Extrapolate(const int* maxConstants) { };
	private: // data
		dbm::dbm_t dbm;
		//TokenMapping mapping;
	};

}

#endif /* DBMMARKING_HPP_ */
