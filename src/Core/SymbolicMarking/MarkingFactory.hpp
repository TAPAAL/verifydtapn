#ifndef MARKINGFACTORY_HPP_
#define MARKINGFACTORY_HPP_

#include <vector>

namespace VerifyTAPN {
	class SymbolicMarking;
	class StoredMarking;

	class MarkingFactory {
	public:
		virtual ~MarkingFactory() { };

		virtual SymbolicMarking* InitialMarking(const std::vector<int>& tokenPlacement) const = 0;
		virtual SymbolicMarking* Clone(const SymbolicMarking& marking) const = 0;
		//virtual SymbolicMarking* Create() const = 0;
		virtual StoredMarking* Convert(SymbolicMarking* marking) const = 0;
		virtual SymbolicMarking* Convert(StoredMarking* marking) const = 0;
	};

}

#endif /* MARKINGFACTORY_HPP_ */
