/*
 * SearchStrategy.hpp
 *
 *  Created on: 10/09/2012
 *      Author: MathiasGS
 */

#ifndef SEARCHSTRATEGY_HPP_
#define SEARCHSTRATEGY_HPP_

namespace VerifyTAPN {
namespace DiscreteVerification {

template <class T>
class SearchStrategy{
public:
	virtual WaitingList<T>* CreateWaitingList(AST::Query* query) const = 0;
	virtual ~SearchStrategy(){};
};

}
}

#endif /* SEARCHSTRATEGY_HPP_ */
