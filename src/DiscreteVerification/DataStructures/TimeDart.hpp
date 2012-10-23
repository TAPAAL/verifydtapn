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

class TimeDart {
public:
	TimeDart(NonStrictMarking* base, int waiting, int passed)
		: base(base), waiting(waiting), passed(passed){
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

private:
	NonStrictMarking* base;
	int waiting;
	int passed;
};

struct TraceDart{
	NonStrictMarking* parent;
	int start;
	int end;
	int successors;

	TraceDart(NonStrictMarking* parent, int start, int end) : parent(parent), start(start), end(end), successors(0){

	}
};

struct WaitingDart{
	TimeDart* dart;
	int w;
	NonStrictMarking* parent;
	int start;
	int end;

	WaitingDart(TimeDart* dart, NonStrictMarking* parent, int w, int start, int end) : dart(dart), w(w), parent(parent), start(start), end(end){

	}
};

}
}

#endif /* TIMEDART_HPP_ */
