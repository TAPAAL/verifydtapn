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
		: base(), waiting(waiting), passed(passed){
		// TODO make base an actual base marking
		NonStrictMarking* baseMarking = new NonStrictMarking(*base);
		int youngest = INT_MAX;
		for(PlaceList::const_iterator place_iter = base->GetPlaceList().begin(); place_iter != base->GetPlaceList().end(); place_iter++){
			if(youngest > place_iter->tokens.front().getAge()){
				youngest = place_iter->tokens.front().getAge();
			}
		}

		if(youngest < INT_MAX && youngest > 0){
			for(PlaceList::iterator place_iter = baseMarking->places.begin(); place_iter != baseMarking->places.end(); place_iter++){
				for(TokenList::iterator token_iter = place_iter->tokens.begin(); token_iter != place_iter->tokens.end(); token_iter++){
					token_iter->setAge(token_iter->getAge()-youngest);
				}
			}
		}

		this->base = *baseMarking;
	}
	~TimeDart(){
		delete &base;
	}

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
