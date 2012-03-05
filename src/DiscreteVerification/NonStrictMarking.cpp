/*
 * NonStrictMarking.cpp
 *
 *  Created on: 29/02/2012
 *      Author: MathiasGS
 */

#include "NonStrictMarking.hpp"
#include <iostream>

namespace VerifyTAPN {
namespace DiscreteVerification {


NonStrictMarking::NonStrictMarking() {
	// TODO Auto-generated constructor stub

}

NonStrictMarking::NonStrictMarking(const std::vector<int>& v){
	for(std::vector<int>::const_iterator iter = v.begin(); iter != v.end(); iter++){
		Token t;
		t.age = 0;
		t.place = *iter;
		placement.push_back(t);
	}
}

int NonStrictMarking::NumberOfTokensInPlace(int placeIndex) const{
	int c = 0;
	for(std::vector<Token>::const_iterator iter = placement.begin(); iter != placement.end(); iter++){
		if(iter->place == placeIndex) c++;
	}
	return c;
}

void NonStrictMarking::MoveToken(unsigned int tokenIndex, int newPlaceIndex){
	assert(tokenIndex >= 0 && tokenIndex < placement.size());
	placement[tokenIndex].place = newPlaceIndex;
}

NonStrictMarking::~NonStrictMarking() {
	// TODO: Should we destruct something here? (placement)
}

bool NonStrictMarking::equals(const NonStrictMarking &m1){
	if(m1.size() == 0) return false;
	if(m1.size() != size())	return false;

	// TODO: extensive equals - should we test more?

	return true;
}

std::ostream& operator<<(std::ostream& out, NonStrictMarking& x ) {
	for(NonStrictMarking::Vector::const_iterator iter = x.placement.begin(); iter != x.placement.end(); iter++){
		out << "(" << iter->age << ", " << iter->place << "), ";
	}

	return out;
}

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
