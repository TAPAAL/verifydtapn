/*
 * NonStrictDFS.cpp
 *
 *  Created on: 05/03/2012
 *      Author: MathiasGS
 */

#include "NonStrictDFS.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

NonStrictDFS::NonStrictDFS(boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options)
	: tapn(tapn), initialMarking(initialMarking), query(query), options(options){

	pwList.Add(initialMarking);
	std::cout << "PWList: " << pwList;
}

bool NonStrictDFS::Verify(){
	//TODO: Implement!

	//Main loop
	while(pwList.HasWaitingStates()){
		NonStrictMarking& marking = pwList.GetNextUnexplored();

		// Do the forall
		addPossibleNextMarkings(marking);

		marking.incrementAge();

		// If not in list, check if goal
		if(pwList.Add(marking)){
			if(false){		// TODO: False should be a check of whether this is what we test for
				return true;
			}
		}
		break;
	}

	return false;
}

void NonStrictDFS::addPossibleNextMarkings(NonStrictMarking& marking){
	for(TAPN::TimedPlace::Vector::const_iterator place_iter = tapn->GetPlaces().begin(); place_iter != tapn->GetPlaces().end(); place_iter++){
		std::cout << place_iter->get()->GetId() << std::endl;
	}
}

NonStrictDFS::~NonStrictDFS() {
	// TODO Auto-generated destructor stub
}

}
}
