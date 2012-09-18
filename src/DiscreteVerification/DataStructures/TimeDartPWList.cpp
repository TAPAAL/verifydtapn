/*
 * PWList.cpp
 *
 *  Created on: 01/03/2012
 *      Author: MathiasGS
 */

#include "TimeDartPWList.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

void TimeDartPWList::Add(TimeDart* dart){
	discoveredMarkings++;
	NonStrictMarkingList& m = markings_storage[dart->getBase().HashKey()];
	for(NonStrictMarkingList::const_iterator iter = m.begin();
			iter != m.end();
			iter++){
		if((*iter)->getBase().equals(dart->getBase())){
			(*iter)->setPassed(min((*iter)->getPassed(),dart->getPassed()));
			(*iter)->setWaiting(min((*iter)->getWaiting(),dart->getWaiting()));

			delete dart;

			if((*iter)->getWaiting() < (*iter)->getPassed()){
				waiting_list->Add((*iter));
			}

			return;
		}
	}

	m.push_back(dart);
	waiting_list->Add(dart);
}

TimeDart* TimeDartPWList::GetNextUnexplored(){
	return waiting_list->Next();
}

TimeDartPWList::~TimeDartPWList() {
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
