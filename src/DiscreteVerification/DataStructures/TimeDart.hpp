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
	TimeDart(NonStrictMarkingBase* base, int waiting, int passed)
		: traceData(NULL), base(base), waiting(waiting), passed(passed){
	}
	~TimeDart(){
	}

	//Getters
	inline NonStrictMarkingBase* getBase(){ return base; }
        inline void setBase(NonStrictMarkingBase* newbase) { base = newbase;}
	inline int getWaiting(){ return waiting; }
	inline int getPassed(){ return passed; }

	//Getters
	inline void setWaiting(int w){ waiting = w; }
	inline void setPassed(int p){ passed = p; }

	TraceMetaDataList* traceData;

private:
	NonStrictMarkingBase* base;
	int waiting;
	int passed;
};

struct WaitingDart{
	TimeDart* dart;
	WaitingDart* parent;
	int w;
	int upper;

	WaitingDart(TimeDart* dart, WaitingDart* parent, int w, int upper) : dart(dart), parent(parent), w(w), upper(upper){

	}

	~WaitingDart(){
			if(parent != NULL && parent->dart->traceData != NULL){
				for(TraceMetaDataList::iterator iter = parent->dart->traceData->begin(); iter != parent->dart->traceData->end(); iter++){
					if((*iter) == this){
						parent->dart->traceData->erase(iter);
						break;
					}
				}
				if(parent->dart->traceData->empty()){
					delete parent->dart->traceData;
					parent->dart->traceData = NULL;
				}
			}
		}
};

struct TraceDart : WaitingDart {
    TAPN::TimedTransition* generatedBy;
    TraceDart(TraceDart &t) : TraceDart(t.dart, t.parent, t.w, t.upper, t.generatedBy){

    };
    TraceDart(TimeDart* dart, WaitingDart* parent, int w, int upper, TAPN::TimedTransition* GeneratedBy) : WaitingDart(dart, parent, w, upper), generatedBy(GeneratedBy){

	}
    
};
}
}

#endif /* TIMEDART_HPP_ */
