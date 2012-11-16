/*
 * NonStrictSearch.cpp
 *
 *  Created on: 26/04/2012
 *      Author: MathiasGS
 */

#include "TimeDartLiveness.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

TimeDartLiveness::TimeDartLiveness(boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<WaitingDart>* waiting_list)
: TimeDartVerification(tapn, options, query, initialMarking), pwList(waiting_list), trace(){}

bool TimeDartLiveness::Verify(){
	if(addToPW(&initialMarking, NULL, 0, INT_MAX)){
		return true;
	}

	//Main loop
	while(pwList.HasWaitingStates()){
		WaitingDart& waitingDart = *pwList.GetNextUnexplored();
		exploredMarkings++;

#ifdef DEBUG
		std::cout << "-----------------------------------------------------------------------------\n";
		std::cout << "Marking: " << *(waitingDart.dart->getBase()) << " waiting: " << waitingDart.dart->getWaiting() << " passed: " << waitingDart.dart->getPassed() << std::endl;
#endif

		int maxCalculatedEnd = -1;

		TraceDart* traceDart = new TraceDart(waitingDart.parent, waitingDart.start, waitingDart.end);

		if(waitingDart.parent != NULL){
			if(waitingDart.parent->traceData == NULL){
				TraceMetaDataList* list = new TraceMetaDataList();
				waitingDart.parent->traceData = list;
			}
			waitingDart.parent->traceData->push_back(traceDart);
		}
		trace.push(traceDart);

		if(canDelayForever(waitingDart.dart->getBase())){
			lastMarking = new TraceList(waitingDart.dart->getBase(), waitingDart.start);
			return true;
		}

		int passed = waitingDart.dart->getPassed();
		waitingDart.dart->setPassed(waitingDart.w);
		for(TimedTransition::Vector::const_iterator transition_iter = tapn->GetTransitions().begin();
				transition_iter != tapn->GetTransitions().end(); transition_iter++){
			TimedTransition& transition = **transition_iter;

#ifdef DEBUG
			std::cout << "Transition: " << transition << std::endl;
#endif

			pair<int,int> calculatedStart = calculateStart(transition, waitingDart.dart->getBase());
			if(calculatedStart.first == -1){	// Transition cannot be enabled in marking
				continue;
			}

			// Aggregate on calculatedEnd for deadlock detection
			if(calculatedStart.second > maxCalculatedEnd){
				maxCalculatedEnd = calculatedStart.second;
			}

			int start = max(waitingDart.w, calculatedStart.first);
			int end = min(passed-1, calculatedStart.second);
			if(start <= end){

				if(transition.GetPostset().size() == 0 || transition.hasUntimedPostset()){
					NonStrictMarking Mpp(*waitingDart.dart->getBase());
					Mpp.incrementAge(start);
					vector<NonStrictMarking*> next = getPossibleNextMarkings(Mpp, transition);
					for(vector<NonStrictMarking*>::iterator it = next.begin(); it != next.end(); it++){
						if(options.GetTrace() == SOME){
							(*it)->SetGeneratedBy(&transition);
						}
						if(addToPW(*it, waitingDart.dart, start, calculatedStart.second)){
							return true;
						}
					}
				}else{
					int stop = max(start, calculateStop(transition, waitingDart.dart->getBase()));
					int finalStop = min(stop, end);
					for(int n = start; n <= finalStop; n++){
						NonStrictMarking Mpp(*waitingDart.dart->getBase());
						Mpp.incrementAge(n);
						int _end = n;
						if(n == stop){
							_end = calculatedStart.second;
						}

						vector<NonStrictMarking*> next = getPossibleNextMarkings(Mpp, transition);
						for(vector<NonStrictMarking*>::iterator it = next.begin(); it != next.end(); it++){
							if(options.GetTrace() == SOME){
								(*it)->SetGeneratedBy(&transition);
							}
							if(addToPW(*it, waitingDart.dart, n, _end)){
								return true;
							}
						}
					}
				}
			}
		}

		if(maxCalculatedEnd < maxPossibleDelay(waitingDart.dart->getBase())){
			lastMarking = new TraceList(waitingDart.dart->getBase(), waitingDart.end);
			return true;	/* DEADLOCK! */
		}

		delete &waitingDart;

		while(!trace.empty() && trace.top()->successors == 0){
			TraceDart* tmp = trace.top();
			trace.pop();
			delete tmp;
			if(trace.empty()){
				return false;
			}
			trace.top()->successors--;
		}
	}

	return false;
}

void TimeDartLiveness::GetTrace(){
	stack<TraceList*> traceStack;

	traceStack.push(lastMarking);
	while(!trace.empty()){
		TraceList* m = new TraceList(trace.top()->parent->getBase(), trace.top()->start);
		traceStack.push(m);
		trace.pop();
	}
	PrintXMLTrace(lastMarking, traceStack, query->GetQuantifier());
}

bool TimeDartLiveness::canDelayForever(NonStrictMarking* marking){
	for(PlaceList::const_iterator p_iter = marking->GetPlaceList().begin(); p_iter != marking->GetPlaceList().end(); p_iter++){
		if(p_iter->place->GetInvariant().GetBound() < INT_MAX){
			return false;
		}
	}
	return true;
}

bool TimeDartLiveness::addToPW(NonStrictMarking* marking, TimeDart* parent, int start, int end){
	marking->cut();

	unsigned int size = marking->size();

	pwList.SetMaxNumTokensIfGreater(size);

	if(size > options.GetKBound()) {
		return false;
	}

	int youngest = marking->makeBase(tapn.get());

	QueryVisitor checker(*marking);
	boost::any context;
	query->Accept(checker, context);
	if(boost::any_cast<bool>(context)) {
		TimeDart* result = pwList.Add(tapn.get(), marking, youngest, parent, start, end);
		if(result != NULL){
			if(!trace.empty()){
				trace.top()->successors++;
			}

			// TODO optimize
			int loop = false;
			if(parent != NULL && parent->getBase()->equals(*result->getBase()) && youngest <= end){
				loop = true;
			}


			//Find the dart created in the PWList
			if(result->traceData != NULL){
				for(TraceMetaDataList::const_iterator iter = result->traceData->begin(); iter != result->traceData->end(); iter++){
					if((*iter)->parent->getBase()->equals(*result->getBase()) && youngest <= (*iter)->end){
						loop = true;
						break;
					}
				}
			}

			if(loop){
				trace.push(new TraceDart(parent, start, end));
				lastMarking = new TraceList(result->getBase(), start);
				return true;
			}
		}
	}

	return false;
}

void TimeDartLiveness::printStats(){
	std::cout << "  discovered markings:\t" << pwList.discoveredMarkings << std::endl;
	std::cout << "  explored markings:\t" << exploredMarkings << std::endl;
	std::cout << "  stored markings:\t" << pwList.Size() << std::endl;
}

TimeDartLiveness::~TimeDartLiveness() {
}

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
