/*
 * IntervalOps.hpp
 *
 *  Created on: 21/09/2012
 *      Author: jakob
 */

#ifndef INTERVALOPS_HPP_
#define INTERVALOPS_HPP_

#include <vector>
#include <limits>

namespace VerifyTAPN {
namespace DiscreteVerification {
namespace Util {

    struct interval {
        int low, high;
        interval(int l, int h) : low(l), high(h) 
        {
            if(low > high)
            {
                low = std::numeric_limits<int>::max();
                high = std::numeric_limits<int>::min();
            }
        };
        auto upper() const {return high; }
        auto lower() const {return low; }
        auto empty() const { return high < low; }
    };

    interval intersect(const interval& l, const interval r);
    interval hull(const interval& l, const interval r);
    bool overlap(const interval& l, const interval r);
    
    
    void setAdd(std::vector< interval >& first, const interval& element);
    std::vector<interval > setIntersection(const std::vector<interval >& first,
                    const std::vector<interval >& second);

} /* namespace Util */
} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* INTERVALOPS_HPP_ */
