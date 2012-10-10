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
using boost::numeric::intersect;

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

		if(!empty(intersection)){
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

void set_add(vector< interval >& first, const interval& element){

	bool inserted = false;
	for(unsigned int i = 0; i < first.size(); i++){

		if(!inserted){
			if(element.upper() < first.at(i).lower()){
				//Add element
				first.insert(first.begin(), element);
				inserted = true;
			} else if(element.lower() >= first.at(i).lower() && element.lower() <= first.at(i).upper()){
				// Merge with node
				int _max = max(first.at(i).upper(), element.upper());
				first.at(i).assign(first.at(i).lower(), _max);
				inserted = true;
				// Clean up
				for(i += 1; i < first.size(); i++){
					if(first.at(i).lower() <= _max){
						// Merge items
						if(first.at(i).upper() >= _max){
							first.at(i-1).assign(first.at(i-1).lower(), first.at(i).upper());
						}
						first.erase(first.begin()+i-1,first.begin()+i);
						i--;
					}
				}
			}
		}else{
			break;
		}
	}

	if(!inserted){
		first.push_back(element);
	}
}

} /* namespace Util */
} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
