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

void set_add(std::vector< boost::numeric::interval<int> >& first, const boost::numeric::interval<int>& element);
void setUnion(std::vector< boost::numeric::interval<int> >& first, const std::vector< boost::numeric::interval<int> >& second);
std::vector<boost::numeric::interval<int> > setIntersection(const std::vector<boost::numeric::interval<int> >& first,
		const std::vector<boost::numeric::interval<int> >& second);

} /* namespace Util */
} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* INTERVALOPS_HPP_ */
