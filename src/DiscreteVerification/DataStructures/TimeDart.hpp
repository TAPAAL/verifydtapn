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
		: base(*(new NonStrictMarking(*base))), waiting(waiting), passed(passed){
		// TODO make base an actual base marking
	}
	virtual ~TimeDart();

	//Getters
	inline NonStrictMarking& getBase(){ return base; }
	inline int getWaiting(){ return waiting; }
	inline int getPassed(){ return passed; }

	//Getters
	inline void setWaiting(int w){ waiting = w; }
	inline void setPassed(int p){ passed = p; }

private:
	NonStrictMarking base;
	int waiting;
	int passed;
};

}
}

#endif /* TIMEDART_HPP_ */
