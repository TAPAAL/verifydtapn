/*
 * PWList.cpp
 *
 *  Created on: 01/03/2012
 *      Author: MathiasGS
 */

#include "PWList.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

PWList::PWList() {
	// TODO Auto-generated constructor stub

}

bool PWList::Add(const NonStrictMarking& marking){
	if(!markings_storage[marking.HashKey()]){
		markings_storage[marking.HashKey()] = marking;
		waiting_list.Add(marking);
	}
}

const NonStrictMarking& PWList::GetNextUnexplored(){
	return waiting_list.Next();
}

PWList::~PWList() {
	// TODO Auto-generated destructor stub
}

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
