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
}

bool NonStrictDFS::Verify(){
	//TODO: Implement!
	if(addToPW(&initialMarking)){
		return true;
	}
	std::cout << "PWList: " << pwList << std::endl;
	std::cout << "MC: " << tapn->MaxConstant() << std::endl;

	//Main loop
	while(pwList.HasWaitingStates()){
		NonStrictMarking& marking = *pwList.GetNextUnexplored();
		std::cout << "Algo: " << marking << std::endl;

		// Do the forall
		vector<NonStrictMarking> next = getPossibleNextMarkings(marking);
		for(vector<NonStrictMarking>::iterator it = next.begin(); it != next.end(); it++){
			if(addToPW(&(*it))){
				return true;
			}
		}

		if(isDelayPossible(marking)){
			marking.incrementAge();
			if(addToPW(&marking)){
				return true;
			}
		}
	}

	return false;
}

vector<NonStrictMarking> NonStrictDFS::getPossibleNextMarkings(NonStrictMarking& marking){
	vector<NonStrictMarking> out;
	for(TAPN::TimedPlace::Vector::const_iterator place_iter = tapn->GetPlaces().begin(); place_iter != tapn->GetPlaces().end(); place_iter++){
		//std::cout << place_iter->get()->GetIndex() << std::endl;
		//Todo: Implement!
	}
	return out;
}

bool NonStrictDFS::addToPW(NonStrictMarking* marking){
	NonStrictMarking* m = cut(*marking);

	if(!isKBound(*m)) return false;

	if(pwList.Add(m)){
		QueryVisitor checker(*m);
		boost::any context;
		query->Accept(checker, context);
		return boost::any_cast<bool>(context);
	}

	return false;
}

NonStrictMarking* NonStrictDFS::cut(NonStrictMarking& marking){
	NonStrictMarking* m = new NonStrictMarking(marking);
	for(PlaceList::iterator place_iter = m->places.begin(); place_iter != m->places.end(); place_iter++){
		for(TokenList::iterator token_iter = place_iter->tokens.begin(); token_iter != place_iter->tokens.end(); token_iter++){
			if(token_iter->getAge() > tapn->MaxConstant()+1){
				token_iter->setAge(tapn->MaxConstant()+1);
			}
		}
	}
	return m;
}

bool NonStrictDFS::isDelayPossible(NonStrictMarking& marking){
	PlaceList& places = marking.places;
	PlaceList::const_iterator markedPlace_iter = places.begin();
	for(TAPN::TimedPlace::Vector::const_iterator place_iter = tapn->GetPlaces().begin(); place_iter != tapn->GetPlaces().end(); place_iter++){
		int inv = place_iter->get()->GetInvariant().GetBound();
		if(place_iter->get()->GetIndex() == markedPlace_iter->id){
			if(markedPlace_iter->NumberOfTokens() > inv-1){
				return false;
			}

			markedPlace_iter++;

			if(markedPlace_iter == places.end())	return true;
		}
	}
	assert(false);	// This happens if there are markings on places not in the TAPN
	return false;
}

bool NonStrictDFS::isKBound(NonStrictMarking& marking){
	return !(marking.size() > options.GetKBound());
}

NonStrictDFS::~NonStrictDFS() {
	// TODO Auto-generated destructor stub
}

}
}
