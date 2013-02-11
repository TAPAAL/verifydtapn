/*
 * NonStrictSearch.cpp
 *
 *  Created on: 26/04/2012
 *      Author: MathiasGS
 */

#include "TimeDartReachabilitySearch.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

bool TimeDartReachabilitySearch::Verify(){
	if(options.GetTrace() == SOME){
		initialMarking.SetGeneratedBy(NULL);
		addToTrace(&initialMarking, NULL, 0);
	}

	if(addToPW(&initialMarking)){
		return true;
		if(options.GetTrace() == SOME){
			lastMarking = &initialMarking;
		}
	}

	//Main loop
	while(pwList->HasWaitingStates()){
		TimeDartBase& dart = *pwList->GetNextUnexplored();
		exploredMarkings++;

		int passed = dart.getPassed();
		dart.setPassed(dart.getWaiting());
		tapn->GetTransitions();
		for(TimedTransition::Vector::const_iterator transition_iter = tapn->GetTransitions().begin();
				transition_iter != tapn->GetTransitions().end(); transition_iter++){
			TimedTransition& transition = **transition_iter;
			pair<int,int> calculatedStart = calculateStart(transition, dart.getBase());
			if(calculatedStart.first == -1){	// Transition cannot be enabled in marking
				continue;
			}
			int start = max(dart.getWaiting(), calculatedStart.first);
			int end = min(passed-1, calculatedStart.second);
			if(start <= end){

				if(transition.hasUntimedPostset()){
					NonStrictMarkingBase Mpp(*dart.getBase());
					Mpp.incrementAge(start);
					vector<NonStrictMarkingBase*> next = getPossibleNextMarkings(Mpp, transition);
					for(vector<NonStrictMarkingBase*>::iterator it = next.begin(); it != next.end(); it++){
						if(addToPW(*it)){
							if(options.GetTrace() == SOME){
								lastMarking = *it;
								(*it)->SetGeneratedBy(&transition);
								addToTrace(*it, dart.getBase(), start);
							}
							return true;
						}
						if(options.GetTrace() == SOME){
							(*it)->SetGeneratedBy(&transition);
							addToTrace(*it, dart.getBase(), start);
						}
					}
				}else{
					int stop = min(max(start, calculateStop(transition, dart.getBase())), end);
					for(int n = start; n <= stop; n++){
						NonStrictMarkingBase Mpp(*dart.getBase());
						Mpp.incrementAge(n);

						vector<NonStrictMarkingBase*> next = getPossibleNextMarkings(Mpp, transition);
						for(vector<NonStrictMarkingBase*>::iterator it = next.begin(); it != next.end(); it++){
							if(addToPW(*it)){
								if(options.GetTrace() == SOME){
									lastMarking = *it;
									(*it)->SetGeneratedBy(&transition);
									addToTrace(*it, dart.getBase(), n);
								}
								return true;
							}
							if(options.GetTrace() == SOME){
								(*it)->SetGeneratedBy(&transition);
								addToTrace(*it, dart.getBase(), n);
							}
						}
					}
				}
			}
		}
                deleteBase(dart.getBase());
	}

	return false;
}

void TimeDartReachabilitySearch::GetTrace(){

	std::cout << "Error generating trace" << std::endl;
	return;

	stack<TraceList*> traceStack;

	TraceList* current = &trace[lastMarking];
	while(current->first != NULL){
		traceStack.push(current);
		current = &trace[current->first];
	}

//	PrintXMLTrace( traceStack, query->GetQuantifier());
}


void TimeDartReachabilitySearch::addToTrace(NonStrictMarkingBase* marking, NonStrictMarkingBase* parent, int d){
	TraceList& m = trace[marking];
	if(m.first == NULL){
		m.first = parent;
		m.second = d;
	}
}

bool TimeDartReachabilitySearch::addToPW(NonStrictMarkingBase* marking){
	marking->cut();

	unsigned int size = marking->size();

	pwList->SetMaxNumTokensIfGreater(size);

	if(size > options.GetKBound()) {
                delete marking;
		return false;
	}

	if(pwList->Add(tapn.get(), marking)){
		QueryVisitor<NonStrictMarkingBase> checker(*marking);
		boost::any context;
		query->Accept(checker, context);
		if(boost::any_cast<bool>(context)) {
			lastMarking = marking;
			return true;
		} else {
                        deleteBase(marking);
			return false;
		}
	}
        deleteBase(marking);
	return false;
}

void TimeDartReachabilitySearch::printStats(){
	std::cout << "  discovered markings:\t" << pwList->discoveredMarkings << std::endl;
	std::cout << "  explored markings:\t" << exploredMarkings << std::endl;
	std::cout << "  stored markings:\t" << pwList->Size() << std::endl;
}

TimeDartReachabilitySearch::~TimeDartReachabilitySearch() {
}

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
