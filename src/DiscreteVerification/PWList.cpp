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

PWList::PWList() {
	// TODO Auto-generated constructor stub

}

void PWList::Add(const NonStrictMarking& marking){
	if(!markings_storage[marking.HashKey()].equals(marking)){
		markings_storage[marking.HashKey()] = marking;
		waiting_list.Add(marking);
	}else{
		std::cout << marking.HashKey() << " exists in PWList and is " << markings_storage[marking.HashKey()] << std::endl;
		std::cout << "Marking is " << marking.size() << " fetch is " << markings_storage[marking.HashKey()].size() << std::endl;
	}
}

const NonStrictMarking& PWList::GetNextUnexplored(){
	//const NonStrictMarking& next = waiting_list.Next();
	//return next;

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
