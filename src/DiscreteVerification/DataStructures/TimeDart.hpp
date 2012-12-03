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

struct WaitingDart;

typedef vector<WaitingDart*> TraceMetaDataList;

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

struct WaitingDart{
	TimeDart* dart;
	TimeDart* parent;
	int w;
	int upper;

	WaitingDart(TimeDart* dart, TimeDart* parent, int w, int upper) : dart(dart), parent(parent), w(w), upper(upper){

	}

	~WaitingDart(){
			if(parent != NULL && parent->traceData != NULL){
				for(TraceMetaDataList::iterator iter = parent->traceData->begin(); iter != parent->traceData->end(); iter++){
					if((*iter) == this){
						parent->traceData->erase(iter);
						break;
					}
				}
				if(parent->traceData->empty()){
					delete parent->traceData;
					parent->traceData = NULL;
				}
			}
		}
};

}
}

#endif /* TIMEDART_HPP_ */
