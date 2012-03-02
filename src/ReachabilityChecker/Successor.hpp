#ifndef SUCCESSOR_HPP_
#define SUCCESSOR_HPP_

#include "Trace/TraceInfo.hpp"

namespace VerifyTAPN
{
	class Successor
	{
	public:
		Successor(SymbolicMarking* marking) : marking(marking), traceInfo(0) { };
		Successor(SymbolicMarking* marking, TraceInfo* traceInfo) : marking(marking), traceInfo(traceInfo) { };

	public:
		SymbolicMarking* Marking() const { return marking; };
		TraceInfo* GetTraceInfo() { return traceInfo; };
	private:
		SymbolicMarking* marking;
		TraceInfo* traceInfo;
	};
}

#endif /* SUCCESSOR_HPP_ */
