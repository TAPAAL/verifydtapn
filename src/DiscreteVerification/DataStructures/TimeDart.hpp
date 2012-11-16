/*
 * TimeDart.hpp
 *
 *  Created on: 18/09/2012
 *      Author: MathiasGS
 */

#ifndef TIMEDART_HPP_
#define TIMEDART_HPP_

#include "NonStrictMarking.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

struct TraceDart;

typedef vector<TraceDart*> TraceMetaDataList;

class TimeDart {
public:
	TimeDart(NonStrictMarking* base, int waiting, int passed)
		: traceData(NULL), base(base), waiting(waiting), passed(passed){
	}
	~TimeDart(){
	}

	//Getters
	inline NonStrictMarking* getBase(){ return base; }
	inline int getWaiting(){ return waiting; }
	inline int getPassed(){ return passed; }

	//Getters
	inline void setWaiting(int w){ waiting = w; }
	inline void setPassed(int p){ passed = p; }

	TraceMetaDataList* traceData;

private:
	NonStrictMarking* base;
	int waiting;
	int passed;
};

struct TraceDart{
	TimeDart* parent;
	int start;
	int end;
	int successors;

	TraceDart(TimeDart* parent, int start, int end) : parent(parent), start(start), end(end), successors(0){

	}

	~TraceDart(){
		for(TraceMetaDataList::iterator iter = parent->traceData->begin(); iter != parent->traceData->end(); iter++){
			if(*iter == this){
				parent->traceData->erase(iter);
			}
		}
		if(parent->traceData->empty()){
			delete parent->traceData;
			parent->traceData = NULL;
		}
	}
};

struct WaitingDart{
	TimeDart* dart;
	int w;
	TimeDart* parent;
	int start;
	int end;

	WaitingDart(TimeDart* dart, TimeDart* parent, int w, int start, int end) : dart(dart), w(w), parent(parent), start(start), end(end){

	}
};

}
}

#endif /* TIMEDART_HPP_ */
