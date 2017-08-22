/*
 * IntervalOps.cpp
 *
 *  Created on: 21/09/2012
 *      Author: jakob
 */

#include "IntervalOps.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {
namespace Util {

using namespace std;

vector<interval > setIntersection(const vector<interval >& first, const vector<interval >& second){
	vector<interval > result;

	if(first.size() < 1 || second.size() < 1){
		return result;
	}

	unsigned int i=0, j=0;

	while(i<first.size() && j<second.size()){
		int i1up = first.at(i).upper();
		int i2up = second.at(j).upper();

		interval intersection = intersect(first.at(i), second.at(j));

		if(!intersection.empty()){
			result.push_back(intersection);
		}

		if(i1up <= i2up ){
			i++;
		}

		if(i2up <= i1up){
			j++;
		}
	}
	return result;
}

interval intersect(const interval& l, const interval r)
{
    if(l.empty()) return l;
    if(r.empty()) return r;
    interval n(std::max(l.low, r.low), std::min(l.high, r.high));
    return n;
}

interval hull(const interval& l, const interval r)
{
    return interval(std::min(l.low, r.low), std::max(l.high, r.high));
}

bool overlap(const interval& l, const interval r)
{
    auto i = intersect(l, r);
    return i.empty();
}

void setAdd(vector< interval >& first, const interval& element){

	for(unsigned int i = 0; i < first.size(); i++){

		if(element.upper() < first.at(i).lower()){
			//Add element
			first.insert(first.begin()+i, element);
			return;
		} else if(overlap(element, first.at(i))){
			interval u = hull(element, first.at(i));
			// Merge with node
			first[i] = u;
			// Clean up
			for(i++; i < first.size(); i++){
				if(first.at(i).lower() <= u.upper()){
					// Merge items
					if(first.at(i).upper() > u.upper()){
						first[i-1] = interval(first.at(i-1).lower(), first.at(i).upper());
					}
					first.erase(first.begin()+i);
					i--;
				}
			}
			return;
		}
	}

	first.push_back(element);
}

} /* namespace Util */
} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
