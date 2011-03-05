#include "EntrySolver.hpp"


namespace VerifyTAPN
{
	void EntrySolver::CalculateDelays()
	{
		CreateLastResetAtLookupTable();
		CreateEntryTimeDBM();
	}

	void EntrySolver::CreateLastResetAtLookupTable()
	{
		unsigned int locations = traceInfos.size()+1;

		if(lraTable != 0) delete[] lraTable;
		lraTable = new unsigned int[tokens*locations]; // TODO: check that this gives correct number of places

		for(unsigned int i = 0; i < tokens; i++)
		{
			lraTable[i] = 0;
		}

		for(unsigned int loc = 1; loc < locations; loc++)
		{
			for(unsigned int clock = 0; clock < tokens; clock++)
			{
				bool isClockUsed = IsClockResetInStep(clock, traceInfos[loc]);
				if(isClockUsed)
					lraTable[loc*tokens+clock] = loc;
				else
					lraTable[loc*tokens+clock] = lraTable[(loc-1)*tokens+clock];
			}
		}
	}

	bool EntrySolver::IsClockResetInStep(unsigned int clock, const TraceInfo& traceInfo) const
	{
		const std::vector<Participant>& participants = traceInfo.Participants();
		for(std::vector<Participant>::const_iterator it = participants.begin(); it != participants.end(); it++)
		{
			if((*it).ClockIndex() == clock) return true;
		}
		return false;
	}

	void EntrySolver::CreateEntryTimeDBM()
	{

	}
}
