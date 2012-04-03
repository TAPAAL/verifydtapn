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
	google::pair<HashMap::const_iterator, HashMap::const_iterator> ret = markings_storage.equal_range(marking->HashKey());
	for(HashMap::const_iterator iter = ret.first;
			ret.second != iter;
			iter++){
		if(iter->second.equals(*marking)){
			return false;
		}
	}
	markings_storage[marking->HashKey()] = *marking;
	waiting_list.Add(marking);
	return true;
}

NonStrictMarking* PWList::GetNextUnexplored(){
	// TODO: Is this really it?
	return waiting_list.Next();
}

PWList::~PWList() {
	// TODO Auto-generated destructor stub
}

std::ostream& operator<<(std::ostream& out, PWList& x){
	out << "Passed and waiting:" << std::endl;
	for(PWList::HashMap::iterator iter = x.markings_storage.begin(); iter != x.markings_storage.end(); iter++){
		out << "- "<< iter->second << std::endl;
	}
	out << "Waiting:" << std::endl << x.waiting_list;
	return out;
}

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
