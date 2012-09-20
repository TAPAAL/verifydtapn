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
		case DEPTHFIRST:
			NonStrictDFS<T> dfs;
			strategy = dfs.CreateWaitingList(query);
			break;
		case RANDOM:
			NonStrictDFSRandom<T> dfsr;
			strategy = dfsr.CreateWaitingList(query);
			break;
		case COVERMOST:
			NonStrictDFSHeuristic<T> dfsh;
			strategy = dfsh.CreateWaitingList(query);
			break;
		default:
			NonStrictDFSHeuristic<T> dfsh2;
			strategy = dfsh2.CreateWaitingList(query);
			break;
		}
	}else if(query->GetQuantifier() == EF || query->GetQuantifier() == AG){
		switch(options.GetSearchType()){
		case DEPTHFIRST:
			NonStrictDFS<T> dfs;
			strategy = dfs.CreateWaitingList(query);
			break;
		case COVERMOST:
			NonStrictDFSHeuristic<T> h;
			strategy = h.CreateWaitingList(query);
			break;
		case BREADTHFIRST:
			NonStrictBFS<T> bfs;
			strategy = bfs.CreateWaitingList(query);
			break;
		case RANDOM:
			NonStrictDFSRandom<T> r;
			strategy = r.CreateWaitingList(query);
			break;
		}
	}
	return strategy;
}

}
}

#endif /* SEARCHFACTORY_H_ */
