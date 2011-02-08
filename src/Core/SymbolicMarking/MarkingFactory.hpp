#ifndef MARKINGFACTORY_HPP_
#define MARKINGFACTORY_HPP_

namespace VerifyTAPN {
	class SymbolicMarking;
	class StoredMarking;

	class MarkingFactory {
	public:
		virtual ~MarkingFactory() { };

		virtual SymbolicMarking* Create() const = 0;
		virtual StoredMarking* Convert(const SymbolicMarking& marking) const = 0;
		virtual SymbolicMarking* Convert(const StoredMarking& marking) const = 0;
	};

}

#endif /* MARKINGFACTORY_HPP_ */
