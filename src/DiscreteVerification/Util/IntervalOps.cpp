/*
 * IntervalOps.cpp
 *
 *  Created on: 21/09/2012
 *      Author: jakob
 */

#include "DiscreteVerification/Util/IntervalOps.hpp"

#include <cassert>
#include <iostream>

namespace VerifyTAPN { namespace DiscreteVerification { namespace Util {

    std::vector<interval> setIntersection(const std::vector<interval> &first, const std::vector<interval> &second) {
        std::vector<interval> result;

        if (first.empty() || second.empty()) {
            return result;
        }

        unsigned int i = 0, j = 0;

        while (i < first.size() && j < second.size()) {
            int i1up = first.at(i).upper();
            int i2up = second.at(j).upper();

            interval intersection = intersect(first.at(i), second.at(j));

            if (!intersection.empty()) {
                result.push_back(intersection);
            }

            if (i1up <= i2up) {
                i++;
            }

            if (i2up <= i1up) {
                j++;
            }
        }
        return result;
    }

    interval intersect(const interval &l, const interval r)
    {
        if (l.empty()) return l;
        if (r.empty()) return r;
        interval n(std::max(l.low, r.low), std::min(l.high, r.high));
        return n;
    }

    interval hull(const interval &l, const interval r) {
        return interval(std::min(l.low, r.low), std::max(l.high, r.high));
    }

    bool overlap(const interval &l, const interval r) {
        auto i = intersect(l, r);
        return !i.empty();
    }

    void setAdd(std::vector<interval> &first, const interval &element) {

        for (unsigned int i = 0; i < first.size(); i++) {

            if (element.upper() < first.at(i).lower()) {
                //Add element
                first.insert(first.begin() + i, element);
                return;
            } else if (overlap(element, first.at(i))) {
                interval u = hull(element, first.at(i));
                // Merge with node
                first[i] = u;
                // Clean up
                for (i++; i < first.size(); i++) {
                    if (first.at(i).lower() <= u.upper()) {
                        // Merge items
                        if (first.at(i).upper() > u.upper()) {
                            first[i - 1] = interval(first.at(i - 1).lower(), first.at(i).upper());
                        }
                        first.erase(first.begin() + i);
                        i--;
                    }
                }
                return;
            }
        }

        first.push_back(element);
    }

    std::vector<interval> complement(const interval &element) {
        const int min_infty = std::numeric_limits<int>::min();
        const int pls_infty = std::numeric_limits<int>::max();
        std::vector<interval> res;
        if(element.empty()) { 
            res.push_back(interval(min_infty, pls_infty));
            return res;
        }
        if(element.lower() != min_infty) {
            res.push_back(interval(min_infty, element.lower() - 1));
        }
        if(element.upper() != pls_infty) {
            res.push_back(interval(element.upper() + 1, pls_infty));
        }
        return res;
    }

    std::vector<interval> setMinus(const std::vector<interval> &first, const interval &element)
    {
        return setIntersection(first, complement(element));
    }

    int setLength(const std::vector<interval> &set) {
        int len = 0;
        for(const auto& i : set) {
            int new_len = i.length();
            if(new_len == std::numeric_limits<int>::max()) {
                return new_len;
            }
            len += new_len;
        }
        return len;
    }

    bool isSetInfinite(const std::vector<interval> &set) {
        if(set.size() == 0) return false;
        return  set.back().upper() == std::numeric_limits<int>::max() ||
                set.front().lower() == std::numeric_limits<int>::min();
    }

    int valueAt(std::vector<interval> &set, int x) {
        if(x < 0) assert(false);
        for(const auto& interv : set) {
            if(x > interv.length()) {
                x -= interv.length();
            } else {
                return x + interv.lower();
            }
        }
        assert(false);
        return 0; // Pathologic, should not happen
    }

    void setDeltaIntoPositive(std::vector<interval> &set, const int dx) {
        for(auto it = set.begin() ; it != set.end() ; it++) {
            it->delta(dx);
            *it = it->positive();
        }
    }

    void printSet(std::vector<interval> &set) {
        for(auto interv : set) {
            std::cout << "[" << interv.lower() << ";" << interv.upper() << "] "; 
        }
        std::cout << std::endl;
    }

    bool contains(std::vector<interval> &set, int value) {
        for(auto interv : set) {
            if(value < interv.lower()) return false;
            if(value <= interv.upper()) return true;
        }
        return false;
    }

} } } /* namespace VerifyTAPN */
