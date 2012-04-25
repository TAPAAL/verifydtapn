/*
 * NonStrictDFS.cpp
 *
 *  Created on: 05/03/2012
 *      Author: MathiasGS
 */

#include "NonStrictDFS.hpp"
#include "PWList.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

bool livenessQuery;

NonStrictDFS::NonStrictDFS(boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options)
	: tapn(tapn), initialMarking(initialMarking), query(query), options(options), successorGenerator( *tapn.get() ){
	livenessQuery = (query->GetQuantifier() == EG || query->GetQuantifier() == AF);
	std::cout<< "livenessQuery: " << (livenessQuery? "true":"false") << std::endl;
}

bool NonStrictDFS::Verify(){
	if(addToPW(&initialMarking)){
		std::cout << "Markings explored: " << pwList.Size() << std::endl;
		return true;
	}

	//Main loop
	while(pwList.HasWaitingStates()){
		NonStrictMarking& next_marking = *pwList.GetNextUnexplored();
		NonStrictMarking marking(next_marking);
		assert(marking.equals(next_marking));
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
			if(addToPW(&(*it))){
				std::cout << "Markings found: " << pwList.Size() << std::endl;
				std::cout << "Markings explored: " << pwList.Size()-pwList.waiting_list.Size() << std::endl;
				return true;
			}
			endOfMaxRun = false;
		}

		if(isDelayPossible(marking)){
			marking.incrementAge();
			if(addToPW(&marking)){
				std::cout << "Markings found: " << pwList.Size() << std::endl;
				std::cout << "Markings explored: " << pwList.Size()-pwList.waiting_list.Size() << std::endl;
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
	}


	std::cout << "Markings explored: " << pwList.Size() << std::endl;
	return false;
}

vector<NonStrictMarking> NonStrictDFS::getPossibleNextMarkings(NonStrictMarking& marking){
	return successorGenerator.generateSuccessors(marking);
}

bool NonStrictDFS::addToPW(NonStrictMarking* marking){
	NonStrictMarking* m = cut(*marking);
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
					delete m;
					if((*iter)->inTrace){
						trace.push(*iter);
						return true;
					}else{
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
			return boost::any_cast<bool>(context);
		} else {
			delete m;
		}
	}

	return false;
}

NonStrictMarking* NonStrictDFS::cut(NonStrictMarking& marking){
	NonStrictMarking* m = new NonStrictMarking(marking);
	for(PlaceList::iterator place_iter = m->places.begin(); place_iter != m->places.end(); place_iter++){
		int count = 0;
#if DEBUG
		std::cout << "Cut before: " << *m << std::endl;
#endif
		for(TokenList::iterator token_iter = place_iter->tokens.begin(); token_iter != place_iter->tokens.end(); token_iter++){
			if(token_iter->getAge() > (tapn->MaxConstant()==0? -1:tapn->MaxConstant())){
				TokenList::iterator beginDelete = token_iter;
				for(; token_iter != place_iter->tokens.end(); token_iter++){
					count += token_iter->getCount();
				}
				m->RemoveRangeOfTokens(*place_iter, beginDelete, place_iter->tokens.end());
				break;
			}
		}
		Token t(tapn->MaxConstant()==0? 0:tapn->MaxConstant()+1,count);
		m->AddTokenInPlace(*place_iter, t);
#if DEBUG
		std::cout << "Cut after: " << *m << std::endl;
#endif
	}
	return m;
}

bool NonStrictDFS::isDelayPossible(NonStrictMarking& marking){
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

bool NonStrictDFS::isKBound(NonStrictMarking& marking){
	return !(marking.size() > options.GetKBound());
}

NonStrictDFS::~NonStrictDFS() {
	// TODO Auto-generated destructor stub
}

}
}
