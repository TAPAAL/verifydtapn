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

                interval(int l, int h) : low(l), high(h) {
                    if (low > high) {
                        low = std::numeric_limits<int>::max();
                        high = std::numeric_limits<int>::min();
                    }
                };

                auto upper() const { return high; }

                auto lower() const { return low; }

                auto empty() const { return high < low; }

                auto length() const { 
                    if(low == std::numeric_limits<int>::min() || high == std::numeric_limits<int>::max()) {
                        return std::numeric_limits<int>::max();
                    }
                    if(empty()) return 0;
                    return high - low; 
                }

                void delta(int dx) {
                    if(empty()) return;
                    if(low != std::numeric_limits<int>::min()) {
                        low += dx;
                    }
                    if(high != std::numeric_limits<int>::max()) {
                        high += dx;
                    }
                }

                interval positive() {
                    if(empty() || high < 0) return interval(1,0);
                    return interval(std::max(low, 0), high);
                }

            };

            interval intersect(const interval &l, const interval r);

            interval hull(const interval &l, const interval r);

            bool overlap(const interval &l, const interval r);


            void setAdd(std::vector<interval> &first, const interval &element);

            std::vector<interval> setIntersection(const std::vector<interval> &first,
                                                  const std::vector<interval> &second);

            std::vector<interval> complement(const interval &element);

            std::vector<interval> setMinus(const std::vector<interval> &first, const interval &element);

            int setLength(const std::vector<interval> &set);

            bool isSetInfinite(const std::vector<interval> &set);

            int valueAt(std::vector<interval> &set, const int x);

            void setDeltaIntoPositive(std::vector<interval> &set, const int dx);

            void printSet(std::vector<interval> &set);

            bool contains(std::vector<interval> &set, int value);

        } /* namespace Util */
    } /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* INTERVALOPS_HPP_ */
