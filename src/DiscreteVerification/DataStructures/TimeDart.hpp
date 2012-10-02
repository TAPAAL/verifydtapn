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

/*std::ostream& operator<<(std::ostream& out, TimeDart& x){
	out << "Base: " << x.getBase() << std::endl << "Waiting: " << x.getWaiting() << std::endl << "Passed: " << x.getPassed() << std::endl;
	return out;
}*/ // TODO fix

}
}

#endif /* TIMEDART_HPP_ */
