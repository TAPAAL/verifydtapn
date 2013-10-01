/*
 * GreatestCommonDivisor.h
 *
 *  Created on: 27/09/2013
 *      Author: jakob
 */

#ifndef GREATESTCOMMONDIVISOR_H_
#define GREATESTCOMMONDIVISOR_H_

#include <algorithm>
#include <stdexcept>

namespace VerifyTAPN {
namespace DiscreteVerification {
namespace Util {

int greatestCommonDivisor(int a, int b);

template <typename ForwardIterator>
int greatestCommonDivisor(ForwardIterator first, ForwardIterator last)
{
	if(first == last){
                return 1;
	}

	int result = *first;
	++first;
	while(first != last){
		result = greatestCommonDivisor(result, *first);
		++first;
	}
	return result;
}

} /* namespace Util */
} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* GREATESTCOMMONDIVISOR_H_ */
