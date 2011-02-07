#ifndef SYMBOLICMARKING_HPP_
#define SYMBOLICMARKING_HPP_

#include "AbstractMarking.hpp"


namespace VerifyTAPN {

	class SymbolicMarking : public AbstractMarking
	{
	public:
		virtual ~SymbolicMarking() { };

		virtual SymbolicMarking* Clone() const = 0;

		virtual void Delay() = 0; // Maybe call this up to match terminology of Down?
		//	virtual void Down() = 0;
		virtual bool IsEmpty() const = 0;
		virtual void Extrapolate(const int* maxConstants) = 0;
	};

}
#endif /* SYMBOLICMARKING_HPP_ */
