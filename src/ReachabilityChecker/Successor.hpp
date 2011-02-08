#ifndef SUCCESSOR_HPP_
#define SUCCESSOR_HPP_

#include "Trace/TraceInfo.hpp"

namespace VerifyTAPN
{
	class Successor
	{
	public:
		Successor(SymbolicMarking* marking) : marking(marking), traceInfo(-1, -1, -1) { };
		Successor(SymbolicMarking* marking, const TraceInfo& traceInfo) : marking(marking), traceInfo(traceInfo) { };
	public:
		SymbolicMarking* Marking() const { return marking; };
		TraceInfo GetTraceInfo() const { return traceInfo; };
	private:
		SymbolicMarking* marking;
		TraceInfo traceInfo;
	};
}

#endif /* SUCCESSOR_HPP_ */
