/*
 * NonStrictSearch.cpp
 *
 *  Created on: 26/04/2012
 *      Author: MathiasGS
 */

#include "NonStrictSearch.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

NonStrictSearch::NonStrictSearch(boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options, WaitingList* waiting_list)
	: tapn(tapn), initialMarking(initialMarking), query(query), options(options), successorGenerator( *tapn.get() ), pwList(waiting_list){
	livenessQuery = (query->GetQuantifier() == EG || query->GetQuantifier() == AF);
	std::cout<< "livenessQuery: " << (livenessQuery? "true":"false") << std::endl;
}

bool NonStrictSearch::Verify(){
	std::cout << "Starting to verify" << std::endl;

	if(addToPW(&initialMarking, NULL)){
		return true;
	}

	//Main loop
	while(pwList.HasWaitingStates()){
		NonStrictMarking& next_marking = *pwList.GetNextUnexplored();
#if DEBUG
		std::cout << "--------------------------Start with new marking-----------------------------------" << std::endl;
		std::cout << "current marking: " << next_marking << " marking size: " << next_marking.size() << std::endl;
#endif
		NonStrictMarking marking(next_marking);

		bool endOfMaxRun = true;
		next_marking.inTrace = true;
		trace.push(&next_marking);
		if(trace.top()->equals(*pwList.markings_storage[trace.top()->HashKey()].at(0))){
			assert(trace.top() == pwList.markings_storage[trace.top()->HashKey()].at(0));
			assert(trace.top()->inTrace);
			assert(pwList.markings_storage[trace.top()->HashKey()].at(0)->inTrace);
		}
		validChildren = 0;

		// Do the forall
		vector<NonStrictMarking> next = getPossibleNextMarkings(marking);
		for(vector<NonStrictMarking>::iterator it = next.begin(); it != next.end(); it++){
#if DEBUG
			std::cout << "Succssor marking: " << *it << std::endl;
#endif
			if(addToPW(&(*it), &next_marking)){
				return true;
			}
			endOfMaxRun = false;
		}

		if(isDelayPossible(marking)){
			marking.incrementAge();
			marking.SetGeneratedBy(NULL);
			if(addToPW(&marking, &next_marking)){
				return true;
			}
			endOfMaxRun = false;
		}

		if(livenessQuery){
			std::cout << "Top marking (" << *trace.top() << ") has children: " << validChildren << std::endl;
			if(endOfMaxRun){
				std::cout << "End of max run" << std::endl;
				return true;
			}
			if(validChildren == 0){
				while(!trace.empty() && trace.top()->children <= 1){
					trace.top()->inTrace = false;
					if(trace.top()->equals(*pwList.markings_storage[trace.top()->HashKey()].at(0))){
						assert(trace.top() == pwList.markings_storage[trace.top()->HashKey()].at(0));
						assert(pwList.markings_storage[trace.top()->HashKey()].at(0)->inTrace == false);
					}
					trace.pop();
				}
				if(trace.empty())	return false;
				trace.top()->children--;
			}else{
				next_marking.children = validChildren;

			}
		}
#if DEBUG
		std::cout << "--------------------------Done with marking-----------------------------------" << std::endl;
#endif
	}


	std::cout << "Markings explored: " << pwList.Size() << std::endl;
	return false;
}

bool NonStrictSearch::isDelayPossible(NonStrictMarking& marking){
	PlaceList& places = marking.places;
	if(places.size() == 0) return false;

	PlaceList::const_iterator markedPlace_iter = places.begin();
	for(TAPN::TimedPlace::Vector::const_iterator place_iter = tapn->GetPlaces().begin(); place_iter != tapn->GetPlaces().end(); place_iter++){
		int inv = place_iter->get()->GetInvariant().GetBound();
		if(*(place_iter->get()) == *(markedPlace_iter->place)){
			if(markedPlace_iter->MaxTokenAge() > inv-1){
				return false;
			}

			markedPlace_iter++;

			if(markedPlace_iter == places.end())	return true;
		}
	}
	assert(false);	// This happens if there are markings on places not in the TAPN
	return false;
}

vector<NonStrictMarking> NonStrictSearch::getPossibleNextMarkings(NonStrictMarking& marking){
	return successorGenerator.generateSuccessors(marking);
}

bool NonStrictSearch::isKBound(NonStrictMarking& marking){
	return !(marking.size() > options.GetKBound());
}

bool NonStrictSearch::addToPW(NonStrictMarking* marking, NonStrictMarking* parent){
	NonStrictMarking* m = cut(*marking);
	m->SetParent(parent);
	assert(m->equals(initialMarking) || m->GetParent() != NULL);
	for(PlaceList::const_iterator it = m->places.begin(); it != m->places.end(); it++){
		for(TokenList::const_iterator iter = it->tokens.begin(); iter != it->tokens.end(); iter++){
			assert(iter->getAge() <= tapn->MaxConstant()+1);
		}
	}

	if(!isKBound(*m)) {
		delete m;
		return false;
	}

	if(livenessQuery){
		QueryVisitor checker(*m);
		boost::any context;
		query->Accept(checker, context);
		if(!boost::any_cast<bool>(context))	return false;
		if(!pwList.Add(m)){
			//Test if collision is in trace
			PWList::NonStrictMarkingList& cm = pwList.markings_storage[m->HashKey()];
			for(PWList::NonStrictMarkingList::iterator iter = cm.begin();
					cm.end() != iter;
					iter++){
				if((*iter)->equals(*m)){
					if((*iter)->inTrace){
						trace.push(m);
						//printStats(pwList);
						return true;
					}else{
						delete m;
						return false;
					}
				}
			}
		}else{
			validChildren++;
		}
	}else{
		if(pwList.Add(m)){
			QueryVisitor checker(*m);
			boost::any context;
			query->Accept(checker, context);
			if(boost::any_cast<bool>(context)) {
				lastMarking = m;
				//printStats(pwList);
				//std::cout << "Markings discovered " << pwList.discoveredMarkings << std::endl;
				//std::cout << "Markings found: " << pwList.Size() << std::endl;
				//std::cout << "Markings explored: " << pwList.Size()-pwList.waiting_list->Size() << std::endl;
				return true;
			} else {
				return false;
			}
		} else {
			delete m;
		}
	}

	return false;
}

NonStrictMarking* NonStrictSearch::cut(NonStrictMarking& marking){
	NonStrictMarking* m = new NonStrictMarking(marking);
	for(PlaceList::iterator place_iter = m->places.begin(); place_iter != m->places.end(); place_iter++){
		const TimedPlace& place = tapn->GetPlace(place_iter->place->GetIndex());
		int count = 0;
#if DEBUG
		std::cout << "Cut before: " << *m << std::endl;
#endif
		for(TokenList::iterator token_iter = place_iter->tokens.begin(); token_iter != place_iter->tokens.end(); token_iter++){
			if(token_iter->getAge() > place.GetMaxConstant()){
				TokenList::iterator beginDelete = token_iter;
				if(place.GetType() == Std){
					for(; token_iter != place_iter->tokens.end(); token_iter++){
						count += token_iter->getCount();
					}
				}
				m->RemoveRangeOfTokens(*place_iter, beginDelete, place_iter->tokens.end());
				break;
			}
		}
		Token t(place.GetMaxConstant()+1,count);
		m->AddTokenInPlace(*place_iter, t);
#if DEBUG
		std::cout << "Cut after: " << *m << std::endl;
#endif
	}
	return m;
}

void NonStrictSearch::printStats(){
	std::cout << "  discovered markings:\t" << pwList.discoveredMarkings << std::endl;
	std::cout << "  explored markings:\t" << pwList.Size() << std::endl;
	std::cout << "  stored markings:\t" << pwList.Size()-pwList.waiting_list->Size() << std::endl;
}

NonStrictSearch::~NonStrictSearch() {
	// TODO Auto-generated destructor stub
}

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
