#ifndef PASSEDWAITINGLIST_HPP_
#define PASSEDWAITINGLIST_HPP_

namespace VerifyTAPN
{
	class PassedWaitingList
	{
	public:
		virtual ~PassedWaitingList() { };

		virtual long long Size() const = 0;
		virtual bool HasWaitingStates() const = 0;

		virtual void Add(const SymMarking& marking) = 0;
		virtual const SymMarking& GetNextUnexplored() = 0;
	};
}

#endif /* PASSEDWAITINGLIST_HPP_ */
