/*
 * PWList.cpp
 *
 *  Created on: 01/03/2012
 *      Author: MathiasGS
 */

#include "PWList.hpp"
#include "NonStrictMarking.hpp"
#include "WaitingList.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

bool PWList::Add(NonStrictMarking* marking){
	if(!markings_storage[marking->HashKey()].equals(*marking)){
		markings_storage[marking->HashKey()] = *marking;
		waiting_list.Add(marking);
		return true;
	}
	return false;
}

NonStrictMarking* PWList::GetNextUnexplored(){
	// TODO: Is this really it?
	return waiting_list.Next();
}

PWList::~PWList() {
	// TODO Auto-generated destructor stub
}

std::ostream& operator<<(std::ostream& out, PWList& x){
	for(PWList::HashMap::iterator iter = x.markings_storage.begin(); iter != x.markings_storage.end(); iter++){
		out << iter->second << " ";
	}
	return out;
}

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
