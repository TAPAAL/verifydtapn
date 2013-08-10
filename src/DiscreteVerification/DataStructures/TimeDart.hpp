/*
 * TimeDart.hpp
 *
 *  Created on: 18/09/2012
 *      Author: MathiasGS
 */

#ifndef TIMEDART_HPP_
#define TIMEDART_HPP_

#include "NonStrictMarking.hpp"
#include "PTrie.h"

namespace VerifyTAPN {
namespace DiscreteVerification {

struct WaitingDart;
struct TraceDart;
class TimeDartBase;

typedef EncodingPointer<TimeDartBase> TimeDartEncodingPointer;  // used for memory-optimization for reachability
typedef EncodingPointer<WaitingDart> WaitingDartEncodingPointer; // mem-optimization for liveness


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

class EncodedReachabilityTraceableDart : public ReachabilityTraceableDart {
public:
    EncodedReachabilityTraceableDart(NonStrictMarkingBase* base, int waiting, int passed) 
        : ReachabilityTraceableDart(base, waiting, passed) {
        
        
    }
    TimeDartEncodingPointer* encoding;
};

class LivenessDart : public TimeDartBase {
public:
    LivenessDart(NonStrictMarkingBase* base, int waiting, int passed)
		: TimeDartBase(base, waiting, passed), traceData(NULL){
        }
    TraceMetaDataList* traceData;
};

class EncodedLivenessDart : public LivenessDart {
public:
    EncodedLivenessDart(NonStrictMarkingBase* base, int waiting, int passed) 
        : LivenessDart(base, waiting, passed) {

    }
    WaitingDartEncodingPointer* encoding;
};

struct WaitingDart{
	TimeDartBase* dart;
	WaitingDart* parent;
	int w;
	int upper;

	WaitingDart(TimeDartBase* dart, WaitingDart* parent, int w, int upper) : dart(dart), parent(parent), w(w), upper(upper){

	}

	~WaitingDart(){
                        
			if(parent != NULL && ((LivenessDart*)parent->dart)->traceData != NULL){
				for(TraceMetaDataList::iterator iter = ((LivenessDart*)parent->dart)->traceData->begin(); iter != ((LivenessDart*)parent->dart)->traceData->end(); iter++){
					if((*iter) == this){
						((LivenessDart*)parent->dart)->traceData->erase(iter);
						break;
					}
				}
				if(((LivenessDart*)parent->dart)->traceData->empty()){
					delete ((LivenessDart*)parent->dart)->traceData;
					((LivenessDart*)parent->dart)->traceData = NULL;
				}
			}
		}
};

struct TraceDart : WaitingDart {
    const TAPN::TimedTransition* generatedBy;
    const int start;
    TraceDart(TraceDart &t) : WaitingDart(t.dart, t.parent, t.w, t.upper), generatedBy(t.generatedBy), start(t.start){
        
    };
    TraceDart(TimeDartBase* dart, WaitingDart* parent, int w, int start, int upper, const TAPN::TimedTransition* GeneratedBy) : WaitingDart(dart, parent, w, upper), generatedBy(GeneratedBy), start(start){

	}
    
};
}
}

#endif /* TIMEDART_HPP_ */
