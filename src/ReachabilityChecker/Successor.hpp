#ifndef SUCCESSOR_HPP_
#define SUCCESSOR_HPP_

#include "Trace/TraceInfo.hpp"

namespace VerifyTAPN
{
	class Successor
	{
	public:
		Successor(SymMarking* marking) : marking(marking), traceInfo(-1, -1, -1) { };
		Successor(SymMarking* marking, const TraceInfo& traceInfo) : marking(marking), traceInfo(traceInfo) { };
	public:
		SymMarking* Marking() const { return marking; };
		TraceInfo GetTraceInfo() const { return traceInfo; };
	private:
		SymMarking* marking;
		TraceInfo traceInfo;
	};
}

#endif /* SUCCESSOR_HPP_ */
