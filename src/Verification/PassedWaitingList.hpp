#ifndef PASSEDWAITINGLIST_HPP_
#define PASSEDWAITINGLIST_HPP_

#include <iostream>

namespace VerifyTAPN
{
	class SymMarking;

	struct Stats{
		long long exploredStates;
		long long discoveredStates;
		long long storedStates;

		Stats() : exploredStates(0), discoveredStates(0), storedStates(0) {};
	};

	class PassedWaitingList
	{
	public:
		virtual ~PassedWaitingList() { };

		virtual long long Size() const = 0;
		virtual bool HasWaitingStates() const = 0;

		virtual bool Add(const SymMarking& marking) = 0;
		virtual SymMarking& GetNextUnexplored() = 0;
		virtual Stats GetStats() const = 0;
		virtual void Print() const = 0;
	};

	inline std::ostream& operator<<(std::ostream& out, const Stats& stats)
	{
		out << "stored states: " << stats.storedStates << "/" << stats.discoveredStates;
		out << ", explored states: " << stats.exploredStates << "/" << stats.storedStates;
		return out;
	}
}

#endif /* PASSEDWAITINGLIST_HPP_ */
