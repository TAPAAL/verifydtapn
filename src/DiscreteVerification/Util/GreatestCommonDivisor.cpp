/*
 * GreatestCommonDivisor.cpp
 *
 *  Created on: 27/09/2013
 *      Author: jakob
 */

#include "GreatestCommonDivisor.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {
namespace Util {

int greatestCommonDivisor(int a, int b){
	if(a < b){
		std::swap(a, b);
	}

	int temp;
	while(b != 0){
		temp = b;
		b = a % b;
		a = temp;
	}

	return a;
}

} /* namespace Util */
} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
