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
struct TraceDart;

typedef vector<WaitingDart*> TraceMetaDataList;

class TimeDartBase {
public:
	TimeDartBase(NonStrictMarkingBase* base, int waiting, int passed)
		: base(base), waiting(waiting), passed(passed){
	}
	~TimeDartBase(){
	}

	//Getters
	inline NonStrictMarkingBase* getBase(){ return base; }
        inline void setBase(NonStrictMarkingBase* newbase) { base = newbase;}
	inline int getWaiting(){ return waiting; }
	inline int getPassed(){ return passed; }

	//Getters
	inline void setWaiting(int w){ waiting = w; }
	inline void setPassed(int p){ passed = p; }

private:
	NonStrictMarkingBase* base;
	int waiting;
	int passed;
};


class ReachabilityTraceableDart : public TimeDartBase {
public:
    ReachabilityTraceableDart(NonStrictMarkingBase* base, int waiting, int passed)
            : TimeDartBase(base, waiting, passed), trace(NULL){
    }
    TraceDart* trace;
};

class LivenessDart : public TimeDartBase {
public:
    LivenessDart(NonStrictMarkingBase* base, int waiting, int passed)
		: TimeDartBase(base, waiting, passed), traceData(NULL){
        }
    TraceMetaDataList* traceData;
};

struct WaitingDart{
	LivenessDart* dart;
	WaitingDart* parent;
	int w;
	int upper;

	WaitingDart(LivenessDart* dart, WaitingDart* parent, int w, int upper) : dart(dart), parent(parent), w(w), upper(upper){

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
    TraceDart(TraceDart &t) : WaitingDart(t.dart, t.parent, t.w, t.upper), generatedBy(t.generatedBy){
        
    };
    TraceDart(LivenessDart* dart, WaitingDart* parent, int w, int upper, TAPN::TimedTransition* GeneratedBy) : WaitingDart(dart, parent, w, upper), generatedBy(GeneratedBy){

	}
    
};
}
}

#endif /* TIMEDART_HPP_ */
