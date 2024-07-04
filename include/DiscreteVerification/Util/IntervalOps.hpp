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
#include <iostream>

namespace VerifyTAPN {
    namespace DiscreteVerification {
        namespace Util {

            template<typename T = int>
            struct interval {
                T low, high;

                static T boundUp() {
                    return std::numeric_limits<T>::has_infinity ?
                        std::numeric_limits<T>::infinity() : std::numeric_limits<T>::max();
                }

                static T boundDown() {
                    return std::numeric_limits<T>::has_infinity ?
                        (-std::numeric_limits<T>::infinity()) : std::numeric_limits<T>::min();
                }

                static T epsilon() {
                    return std::numeric_limits<T>::is_integer ?
                        1 : std::numeric_limits<T>::epsilon();
                }

                interval(T l, T h) : low(l), high(h) {
                    if (low > high) {
                        low = boundUp();
                        high = boundDown();
                    }
                };

                auto upper() const { return high; }

                auto lower() const { return low; }

                auto empty() const { return high < low; }

                auto length() const { 
                    if(low == boundDown() || high == boundUp()) {
                        return boundUp();
                    }
                    if(empty()) return (T) 0;
                    return high - low; 
                }

                void delta(T dx) {
                    if(empty()) return;
                    if(low != boundDown()) {
                        low += dx;
                    }
                    if(high != boundUp()) {
                        high += dx;
                    }
                }

                interval positive() {
                    if(empty() || high < 0) return interval((T) 1,0);
                    return interval(std::max(low, (T) 0), high);
                }

            };

            template<typename T = int>
            interval<T> intersect(const interval<T> &l, const interval<T> r) {
                if (l.empty()) return l;
                if (r.empty()) return r;
                interval n(std::max(l.low, r.low), std::min(l.high, r.high));
                return n;
            }

            template<typename T = int>
            interval<T> hull(const interval<T> &l, const interval<T> r) {
                return interval(std::min(l.low, r.low), std::max(l.high, r.high));
            }

            template<typename T = int>
            bool overlap(const interval<T> &l, const interval<T> r) {
                auto i = intersect(l, r);
                return !i.empty();
            }

            template<typename T = int>
            void setAdd(std::vector<interval<T>> &first, const interval<T> &element) {
                for (unsigned int i = 0; i < first.size(); i++) {

                    if (element.upper() < first.at(i).lower()) {
                        //Add element
                        first.insert(first.begin() + i, element);
                        return;
                    } else if (overlap(element, first.at(i))) {
                        interval<T> u = hull(element, first.at(i));
                        // Merge with node
                        first[i] = u;
                        // Clean up
                        for (i++; i < first.size(); i++) {
                            if (first.at(i).lower() <= u.upper()) {
                                // Merge items
                                if (first.at(i).upper() > u.upper()) {
                                    first[i - 1] = interval<T>(first.at(i - 1).lower(), first.at(i).upper());
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

            template<typename T = int>
            std::vector<interval<T>> setIntersection(const std::vector<interval<T>> &first,
                                                  const std::vector<interval<T>> &second) {
                std::vector<interval<T>> result;

                if (first.empty() || second.empty()) {
                    return result;
                }

                unsigned int i = 0, j = 0;

                while (i < first.size() && j < second.size()) {
                    int i1up = first.at(i).upper();
                    int i2up = second.at(j).upper();

                    interval<T> intersection = intersect(first.at(i), second.at(j));

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

            template<typename T = int>
            std::vector<interval<T>> complement(const interval<T> &element) {
                const int min_infty = interval<T>::boundDown();
                const int pls_infty = interval<T>::boundUp();
                std::vector<interval<T>> res;
                if(element.empty()) { 
                    res.push_back(interval<T>(min_infty, pls_infty));
                    return res;
                }
                if(element.lower() != min_infty) {
                    res.push_back(interval<T>(min_infty, element.lower() - interval<T>::epsilon()));
                }
                if(element.upper() != pls_infty) {
                    res.push_back(interval<T>(element.upper() + interval<T>::epsilon(), pls_infty));
                }
                return res;
            }

            template<typename T = int>
            std::vector<interval<T>> setMinus(const std::vector<interval<T>> &first, const interval<T> &element) {
                return setIntersection(first, complement(element));
            }

            template<typename T = int>
            T setLength(const std::vector<interval<T>> &set) {
                T len = 0;
                for(const auto& i : set) {
                    T new_len = i.length();
                    if(new_len == interval<T>::boundUp()) {
                        return new_len;
                    }
                    len += new_len;
                }
                return len;
            } 

            template<typename T = int>
            bool isSetInfinite(const std::vector<interval<T>> &set) {
                if(set.size() == 0) return false;
                return  set.back().upper() == interval<T>::boundUp() ||
                        set.front().lower() == interval<T>::boundDown();
            }

            template<typename T = int>
            T valueAt(std::vector<interval<T>> &set, const T x) {
                if(x < 0) return interval<T>::boundDown();
                for(const auto& interv : set) {
                    if(x > interv.length()) {
                        x -= interv.length();
                    } else {
                        return x + interv.lower();
                    }
                }
                return interval<T>::boundUp();
            }

            template<typename T = int>
            void setDeltaIntoPositive(std::vector<interval<T>> &set, const T dx) {
                for(auto it = set.begin() ; it != set.end() ; it++) {
                    it->delta(dx);
                    *it = it->positive();
                }
            }

            template<typename T = int>
            void printSet(std::vector<interval<T>> &set) {
                for(auto interv : set) {
                    std::cout << "[" << interv.lower() << ";" << interv.upper() << "] "; 
                }
                std::cout << std::endl;
            }

            template<typename T = int>
            bool contains(std::vector<interval<T>> &set, T value) {
                for(auto interv : set) {
                    if(value < interv.lower()) return false;
                    if(value <= interv.upper()) return true;
                }
                return false;
            }

        } /* namespace Util */
    } /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* INTERVALOPS_HPP_ */
