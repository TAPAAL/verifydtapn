/*
 * SearchFactory.h
 *
 *  Created on: 20/09/2012
 *      Author: jakob
 */

#ifndef SEARCHFACTORY_H_
#define SEARCHFACTORY_H_

#include "SearchStrategies.hpp"
#include "../DataStructures/WaitingList.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

template <class T>
WaitingList<T>* getWaitingList(AST::Query* query, VerificationOptions& options){
	WaitingList<T>* strategy = NULL;
	if(options.getWorkflowMode() == options.WORKFLOW_SOUNDNESS){
		WorkflowMinFirst<T> s;
		strategy = s.createWaitingList(query);
	} if(options.getWorkflowMode() == options.WORKFLOW_STRONG_SOUNDNESS){
            NonStrictDFS<T> s;
            strategy = s.createWaitingList(query);
        } else if(query->getQuantifier() == EG || query->getQuantifier() == AF){
		//Liveness query, force DFS
		switch(options.getSearchType()){
		case VerificationOptions::DEPTHFIRST: {
			NonStrictDFS<T> s;
			strategy = s.createWaitingList(query);
			break;
		}
		case VerificationOptions::RANDOM:{
			NonStrictDFSRandom<T> s;
			strategy = s.createWaitingList(query);
			break;
		}
		case VerificationOptions::COVERMOST: {
			NonStrictDFSHeuristic<T> s;
			strategy = s.createWaitingList(query);
			break;
		}
		case VerificationOptions::MINDELAYFIRST:{
			WorkflowMinFirst<T> s;
			strategy = s.createWaitingList(query);
			break;
		}
		default: {
			NonStrictDFSHeuristic<T> s;
			strategy = s.createWaitingList(query);
			break;
		}
		}
	}else if(query->getQuantifier() == EF || query->getQuantifier() == AG){
		switch(options.getSearchType()){
		case VerificationOptions::DEPTHFIRST: {
			NonStrictDFS<T> s;
			strategy = s.createWaitingList(query);
			break;
		}
		case VerificationOptions::COVERMOST: {
			NonStrictHeuristic<T> s;
			strategy = s.createWaitingList(query);
			break;
		}
		case VerificationOptions::BREADTHFIRST: {
			NonStrictBFS<T> s;
			strategy = s.createWaitingList(query);
			break;
		}
		case VerificationOptions::RANDOM: {
			NonStrictDFSRandom<T> s;
			strategy = s.createWaitingList(query);
			break;
		}
		case VerificationOptions::MINDELAYFIRST:{
			WorkflowMinFirst<T> s;
			strategy = s.createWaitingList(query);
			break;
		}
                default: {
			NonStrictHeuristic<T> s;
			strategy = s.createWaitingList(query);
			break;
		}
		}
	}
	return strategy;
}

}
}

#endif /* SEARCHFACTORY_H_ */
