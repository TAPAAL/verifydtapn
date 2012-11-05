/*
 * PWList.cpp
 *
 *  Created on: 01/03/2012
 *      Author: MathiasGS
 */

#include "TimeDartLivenessPWList.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

bool TimeDartLivenessPWList::Add(TAPN::TimedArcPetriNet* tapn, NonStrictMarking* marking, int youngest, NonStrictMarking* parent, int start, int end){
	discoveredMarkings++;
	TimeDartList& m = markings_storage[marking->HashKey()];
	for(TimeDartList::const_iterator iter = m.begin();
			iter != m.end();
			iter++){
		if((*iter)->getBase()->equals(*marking)){
			(*iter)->setWaiting(min((*iter)->getWaiting(),youngest));

			if((*iter)->getWaiting() < (*iter)->getPassed()){
				waiting_list->Add(new WaitingDart((*iter), parent, youngest, start, end));
				delete marking;
				return true;
			}

			delete marking;

			return false;
		}
	}

	TimeDart* dart = new TimeDart(marking, youngest, INT_MAX);
	m.push_back(dart);
	waiting_list->Add(new WaitingDart(dart, parent, youngest, start, end));
	return true;
}

WaitingDart* TimeDartLivenessPWList::GetNextUnexplored(){
	return waiting_list->Next();
}

TimeDartLivenessPWList::~TimeDartLivenessPWList() {
	// TODO Auto-generated destructor stub
}

std::ostream& operator<<(std::ostream& out, TimeDartLivenessPWList& x){
	out << "Passed and waiting:" << std::endl;
	for(TimeDartLivenessPWList::HashMap::iterator iter = x.markings_storage.begin(); iter != x.markings_storage.end(); iter++){
		for(TimeDartLivenessPWList::TimeDartList::iterator m_iter = iter->second.begin(); m_iter != iter->second.end(); m_iter++){
			out << "- "<< *m_iter << std::endl;
		}
	}
	out << "Waiting:" << std::endl << x.waiting_list;
	return out;
}

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
