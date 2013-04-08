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
WaitingList<T>* GetWaitingList(AST::Query* query, VerificationOptions& options){
	WaitingList<T>* strategy = NULL;
	if(query->GetQuantifier() == EG || query->GetQuantifier() == AF){
		//Liveness query, force DFS
		switch(options.GetSearchType()){
		case DEPTHFIRST: {
			NonStrictDFS<T> s;
			strategy = s.CreateWaitingList(query);
			break;
		}
		case RANDOM:{
			NonStrictDFSRandom<T> s;
			strategy = s.CreateWaitingList(query);
			break;
		}
		case COVERMOST: {
			NonStrictDFSHeuristic<T> s;
			strategy = s.CreateWaitingList(query);
			break;
		}
		default: {
			NonStrictDFSHeuristic<T> s;
			strategy = s.CreateWaitingList(query);
			break;
		}
		}
	}else if(query->GetQuantifier() == EF || query->GetQuantifier() == AG){
		switch(options.GetSearchType()){
		case DEPTHFIRST: {
			NonStrictDFS<T> s;
			strategy = s.CreateWaitingList(query);
			break;
		}
		case COVERMOST: {
			NonStrictHeuristic<T> s;
			strategy = s.CreateWaitingList(query);
			break;
		}
		case BREADTHFIRST: {
			NonStrictBFS<T> s;
			strategy = s.CreateWaitingList(query);
			break;
		}
		case RANDOM: {
			NonStrictDFSRandom<T> s;
			strategy = s.CreateWaitingList(query);
			break;
		}
		}
	}
	return strategy;
}

}
}

#endif /* SEARCHFACTORY_H_ */
