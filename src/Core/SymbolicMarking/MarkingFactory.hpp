#ifndef MARKINGFACTORY_HPP_
#define MARKINGFACTORY_HPP_

namespace VerifyTAPN {
	class SymbolicMarking;
	class StoredMarking;
	class DiscretePart;

	class MarkingFactory {
	public:
		virtual ~MarkingFactory() { };

		virtual SymbolicMarking* InitialMarking(const DiscretePart& dp) const = 0;
		//virtual SymbolicMarking* Clone(const SymbolicMarking& marking) const = 0;
		//virtual SymbolicMarking* Create() const = 0;
		virtual StoredMarking* Convert(SymbolicMarking* marking) const = 0;
		virtual SymbolicMarking* Convert(StoredMarking* marking) const = 0;
	};

}

#endif /* MARKINGFACTORY_HPP_ */
