/*
 * SearchStrategy.hpp
 *
 *  Created on: 10/09/2012
 *      Author: MathiasGS
 */

// #ifndef SEARCHSTRATEGY_HPP_
// #define SEARCHSTRATEGY_HPP_

namespace VerifyTAPN {
namespace DiscreteVerification {

#include "NonStrictDFS.hpp"
#include "NonStrictBFS.hpp"
#include "NonStrictDFSHeuristic.hpp"
#include "NonStrictDFSRandom.hpp"
#include "NonStrictHeuristic.hpp"
#include "NonStrictRandom.hpp"

template <class T>
class SearchStrategy{
public:
	static WaitingList<T>* GetWaitingList(AST::Query* query, VerificationOptions& options);
	virtual ~SearchStrategy(){};
	virtual WaitingList<T>* CreateWaitingList(AST::Query* query) const = 0;
};

template <class T>
WaitingList<T>* SearchStrategy<T>::GetWaitingList(AST::Query* query, VerificationOptions& options){
	WaitingList<T>* strategy = NULL;/*
	if(query->GetQuantifier() == EG || query->GetQuantifier() == AF){
		//Liveness query, force DFS
		switch(options.GetSearchType()){
		case DEPTHFIRST:
			NonStrictDFS<T> s;
			strategy = s.CreateWaitingList(query);
			break;
		case RANDOM:
			strategy = (new NonStrictDFSRandom())->CreateWaitingList(query);
			break;
		case COVERMOST:
			strategy = (new NonStrictDFSHeuristic())->CreateWaitingList(query);
			break;
		default:
			strategy = (new NonStrictDFSHeuristic())->CreateWaitingList(query);
			break;
		}
	}else if(query->GetQuantifier() == EF || query->GetQuantifier() == AG){
		switch(options.GetSearchType()){
		case DEPTHFIRST:
			strategy = (new NonStrictDFS())->CreateWaitingList(query);
			break;
		case COVERMOST:
			strategy = (new NonStrictHeuristic())->CreateWaitingList(query);
			break;
		case BREADTHFIRST:
			strategy = (new NonStrictBFS())->CreateWaitingList(query);
			break;
		case RANDOM:
			strategy = (new NonStrictRandom())->CreateWaitingList(query);
			break;
		}
	}*/
	return NULL;
}

}
}

// #endif /* SEARCHSTRATEGY_HPP_ */
