/*
 * PWList.cpp
 *
 *  Created on: 01/03/2012
 *      Author: MathiasGS
 */

#include "PWList.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

bool PWList::Add(NonStrictMarking* marking){
	discoveredMarkings++;
	NonStrictMarkingList& m = markings_storage[marking->HashKey()];
	for(NonStrictMarkingList::const_iterator iter = m.begin();
			iter != m.end();
			iter++){
		if((*iter)->equals(*marking)){
			if(!(*iter)->passed){
				waiting_list->Add(*iter, *iter);
			}
			return false;
		}
	}
	m.push_back(marking);
	waiting_list->Add(marking, marking);
	return true;
}

NonStrictMarking* PWList::GetNextUnexplored(){
	return waiting_list->Pop();
}

PWList::~PWList() {
	// TODO Auto-generated destructor stub
}

std::ostream& operator<<(std::ostream& out, PWList& x){
	out << "Passed and waiting:" << std::endl;
	for(PWList::HashMap::iterator iter = x.markings_storage.begin(); iter != x.markings_storage.end(); iter++){
		for(PWList::NonStrictMarkingList::iterator m_iter = iter->second.begin(); m_iter != iter->second.end(); m_iter++){
			out << "- "<< *m_iter << std::endl;
		}
	}
	out << "Waiting:" << std::endl << x.waiting_list;
	return out;
}

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
