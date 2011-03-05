#ifndef ENTRYSOLVER_HPP_
#define ENTRYSOLVER_HPP_

#include "TraceInfo.hpp"
#include <deque>
#include "dbm/fed.h"

namespace VerifyTAPN
{
	class EntrySolver
	{
	public:
		EntrySolver(unsigned int tokens, const std::deque<TraceInfo>& traceInfos) : lraTable(0), entryTimeDBM(traceInfos.size()+1), tokens(tokens), traceInfos(traceInfos) { };
		virtual ~EntrySolver() { delete[] lraTable; };

		void CalculateDelays();
	private:
		inline unsigned int LastResetAt(unsigned int clock, unsigned int location) const { return lraTable[location*tokens+clock]; };

		void CreateLastResetAtLookupTable();
		void CreateEntryTimeDBM();

		bool IsClockResetInStep(unsigned int clock, const TraceInfo& traceInfo) const;

	private:
		unsigned int* lraTable;
		dbm::dbm_t entryTimeDBM;

		unsigned int tokens;
		const std::deque<TraceInfo>& traceInfos;
	};
}

#endif /* ENTRYSOLVER_HPP_ */
