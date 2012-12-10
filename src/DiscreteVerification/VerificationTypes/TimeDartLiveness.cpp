/*
 * NonStrictSearch.cpp
 *
 *  Created on: 26/04/2012
 *      Author: MathiasGS
 */

#include "TimeDartLiveness.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

bool TimeDartLiveness::Verify(){
	if(addToPW(&initialMarking, NULL, INT_MAX)){
		return true;
	}

	//Main loop
	while(pwList->HasWaitingStates()){
		WaitingDart& waitingDart = *pwList->GetNextUnexplored();
		exploredMarkings++;

		// Add trace meta data ("add to trace")
		if(waitingDart.parent != NULL){
			if(waitingDart.parent->traceData == NULL){
				TraceMetaDataList* list = new TraceMetaDataList();
				waitingDart.parent->traceData = list;
			}
			waitingDart.parent->traceData->push_back(&waitingDart);
		}

		// Detect ability to delay forever
		if(canDelayForever(waitingDart.dart->getBase())){
			NonStrictMarkingBase* lm = new NonStrictMarkingBase(*waitingDart.dart->getBase());
			lm->generatedBy = waitingDart.dart->getBase()->generatedBy;
			lastMarking = new TraceList(lm, waitingDart.upper);
			return true;
		}

		// Initialize
		int maxCalculatedEnd = -1;

		// Set passed
		int passed = waitingDart.dart->getPassed();

		// Skip if already passed
		if(passed <= waitingDart.w){
			if(waitingDart.parent != NULL){
				waitingDart.parent->traceData->pop_back();
			}
			pwList->PopWaiting();
			continue;
		}

		waitingDart.dart->setPassed(waitingDart.w);

		// Iterate over transitions
		for(TimedTransition::Vector::const_iterator transition_iter = tapn->GetTransitions().begin();
				transition_iter != tapn->GetTransitions().end(); transition_iter++){
			TimedTransition& transition = **transition_iter;

			// Calculate enabled set
			pair<int,int> calculatedStart = calculateStart(transition, waitingDart.dart->getBase());
			if(calculatedStart.first == -1){	// Transition cannot be enabled in marking
				continue;
			}

			// Aggregate on calculatedEnd for deadlock detection
			if(calculatedStart.second > maxCalculatedEnd){
				maxCalculatedEnd = calculatedStart.second;
			}

			// Calculate start and end
			int start = max(waitingDart.w, calculatedStart.first);
			int end = min(passed-1, calculatedStart.second);
			if(start <= end){
				int stop = max(start, calculateStop(transition, waitingDart.dart->getBase()));
				int finalStop = min(stop, end);
				for(int n = start; n <= finalStop; n++){
					NonStrictMarkingBase Mpp(*waitingDart.dart->getBase());
					Mpp.incrementAge(n);
					int _end = n;
					if(n == stop){
						_end = calculatedStart.second;
					}

					vector<NonStrictMarkingBase*> next = getPossibleNextMarkings(Mpp, transition);
					for(vector<NonStrictMarkingBase*>::iterator it = next.begin(); it != next.end(); it++){
						if(options.GetTrace() == SOME){
							(*it)->SetGeneratedBy(&transition);
						}
						if(addToPW(*it, waitingDart.dart, _end)){
							lastMarking->first->generatedBy = &transition;
							return true;
						}
					}

					pwList->flushBuffer();
				}
			}
		}

		// Detect deadlock
		if(maxCalculatedEnd < maxPossibleDelay(waitingDart.dart->getBase())){
			lastMarking = new TraceList(waitingDart.dart->getBase(), maxPossibleDelay(waitingDart.dart->getBase()));
			return true;	/* DEADLOCK! */
		}
	}

	return false;
}

void TimeDartLiveness::GetTrace(){
	stack<TraceList*> traceStack;

	std::cout << "Not implemented" << std::endl;

	#if DEBUG
		//std::cout << "Trace size: " << trace.size() << std::endl;
		std::cout << "Last marking: " << *lastMarking->first << std::endl;
	#endif

	/*traceStack.push(lastMarking);
	WaitingDart* parent = pwList.GetNextUnexplored()->parent->;
	while(parent != NULL){
		TraceList* m = new TraceList(parent->dart->getBase(), parent->upper);
		traceStack.push(m);
		parent = parent->parent;
	}

	PrintXMLTrace(lastMarking, traceStack, query->GetQuantifier());*/
}

bool TimeDartLiveness::canDelayForever(NonStrictMarkingBase* marking){
	for(PlaceList::const_iterator p_iter = marking->GetPlaceList().begin(); p_iter != marking->GetPlaceList().end(); p_iter++){
		if(p_iter->place->GetInvariant().GetBound() < INT_MAX){
			return false;
		}
	}
	return true;
}

bool TimeDartLiveness::addToPW(NonStrictMarkingBase* marking, TimeDart* parent, int upper){
	marking->cut();

	unsigned int size = marking->size();

	pwList->SetMaxNumTokensIfGreater(size);

	if(size > options.GetKBound()) {
		return false;
	}

	int youngest = marking->makeBase(tapn.get());

	QueryVisitor<NonStrictMarkingBase> checker(*marking);
	boost::any context;
	query->Accept(checker, context);
	if(boost::any_cast<bool>(context)) {
		std::pair<TimeDart*, bool> result = pwList->Add(tapn.get(), marking, youngest, parent, upper);

		int loop = false;
		if(parent != NULL && parent->getBase()->equals(*result.first->getBase()) && youngest <= upper){
			loop = true;
		}

		//Find the dart created in the PWList
		if(result.first->traceData != NULL){
			for(TraceMetaDataList::const_iterator iter = result.first->traceData->begin(); iter != result.first->traceData->end(); iter++){
				if((*iter)->parent->getBase()->equals(*result.first->getBase()) && youngest <= (*iter)->upper){
					loop = true;
					break;
				}
			}
		}

		if(loop){
			NonStrictMarkingBase* lm = new NonStrictMarkingBase(*result.first->getBase());
			lm->parent = parent->getBase();
			lastMarking = new TraceList(lm, upper);
			return true;
		}
	}

	return false;
}

void TimeDartLiveness::printStats(){
	std::cout << "  discovered markings:\t" << pwList->discoveredMarkings << std::endl;
	std::cout << "  explored markings:\t" << exploredMarkings << std::endl;
	std::cout << "  stored markings:\t" << pwList->Size() << std::endl;
}

TimeDartLiveness::~TimeDartLiveness() {
}

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
