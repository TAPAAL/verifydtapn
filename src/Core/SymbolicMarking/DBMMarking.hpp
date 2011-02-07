#ifndef DBMMARKING_HPP_
#define DBMMARKING_HPP_

#include "DiscreteMarking.hpp"
#include "TokenMapping.hpp"
#include "dbm/fed.h"

namespace VerifyTAPN {

	class DBMMarking: public DiscreteMarking {
	public:
		DBMMarking(const DiscretePart& dp, const dbm::dbm_t& dbm) : DiscreteMarking(dp), dbm(dbm), mapping() { InitMapping(); };
		DBMMarking(const DBMMarking& dm) : DiscreteMarking(dm), dbm(dm.dbm), mapping(dm.mapping) { };
		virtual ~DBMMarking() { };

		virtual DBMMarking* Clone() const { return new DBMMarking(*this); };

		virtual void Reset(int token) { dbm(mapping.GetMapping(token)) = 0; };
		virtual bool IsEmpty() const { return dbm.isEmpty(); };
		virtual void Delay() { dbm.up(); };
		virtual void Constrain(int token, const TAPN::TimeInterval& interval)
		{
			int clock = mapping.GetMapping(token);
			dbm.constrain(0,clock, interval.LowerBoundToDBMRaw());
			dbm.constrain(clock, 0, interval.UpperBoundToDBMRaw());
		};

		virtual bool PotentiallySatisfies(int token, const TAPN::TimeInterval& interval) const
		{
			int clock = mapping.GetMapping(token);
			bool isLowerBoundSat = dbm.satisfies(0, clock, interval.LowerBoundToDBMRaw());
			bool isUpperBoundSat = dbm.satisfies(clock, 0, interval.UpperBoundToDBMRaw());
			bool inappropriateAge = !isLowerBoundSat || !isUpperBoundSat;
			return !inappropriateAge;
		};

		virtual void Extrapolate(const int* maxConstants) { };

		virtual void AddTokens(const std::list<int>& placeIndices);
		virtual void RemoveTokens(const std::vector<int>& tokenIndices);
	private:
		void InitMapping();

	private: // data
		dbm::dbm_t dbm;
		TokenMapping mapping;
	};

}

#endif /* DBMMARKING_HPP_ */
