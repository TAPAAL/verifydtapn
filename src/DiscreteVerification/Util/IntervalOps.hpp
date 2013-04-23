/*
 * IntervalOps.hpp
 *
 *  Created on: 21/09/2012
 *      Author: jakob
 */

#ifndef INTERVALOPS_HPP_
#define INTERVALOPS_HPP_

#include <vector>
#include "boost/numeric/interval.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {
namespace Util {

typedef boost::numeric::interval_lib::rounded_math< int > rounding_policy;
typedef boost::numeric::interval_lib::checking_no_nan< int > checking_policy;
typedef boost::numeric::interval_lib::policies< rounding_policy, checking_policy > interval_policies;
typedef boost::numeric::interval< int, interval_policies > interval;

void setAdd(std::vector< interval >& first, const interval& element);
std::vector<interval > setIntersection(const std::vector<interval >& first,
		const std::vector<interval >& second);

} /* namespace Util */
} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* INTERVALOPS_HPP_ */
