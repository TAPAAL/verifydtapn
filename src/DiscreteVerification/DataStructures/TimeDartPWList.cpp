/*
 * PWList.cpp
 *
 *  Created on: 01/03/2012
 *      Author: MathiasGS
 */

#include "TimeDartPWList.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

bool TimeDartPWList::Add(TAPN::TimedArcPetriNet* tapn, NonStrictMarking* marking){
	discoveredMarkings++;
	int youngest = marking->makeBase(tapn);
	NonStrictMarkingList& m = markings_storage[marking->HashKey()];
	for(NonStrictMarkingList::const_iterator iter = m.begin();
			iter != m.end();
			iter++){
		if((*iter)->getBase()->equals(*marking)){
				bool inWaiting = (*iter)->getWaiting() < (*iter)->getPassed();

				(*iter)->setWaiting(min((*iter)->getWaiting(),youngest));

				if((*iter)->getWaiting() < (*iter)->getPassed() && !inWaiting){
					waiting_list->Add((*iter));
				}

			delete marking;

			return false;
		}
	}

	TimeDart* dart = new TimeDart(marking, youngest, INT_MAX);
	m.push_back(dart);
	waiting_list->Add(dart);
	return true;
}

TimeDart* TimeDartPWList::GetNextUnexplored(){
	return waiting_list->Pop();
}

TimeDartPWList::~TimeDartPWList() {
	// TODO Auto-generated destructor stub
}

std::ostream& operator<<(std::ostream& out, TimeDartPWList& x){
	out << "Passed and waiting:" << std::endl;
	for(TimeDartPWList::HashMap::iterator iter = x.markings_storage.begin(); iter != x.markings_storage.end(); iter++){
		for(TimeDartPWList::NonStrictMarkingList::iterator m_iter = iter->second.begin(); m_iter != iter->second.end(); m_iter++){
			out << "- "<< *m_iter << std::endl;
		}
	}
	out << "Waiting:" << std::endl << x.waiting_list;
	return out;
}

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
