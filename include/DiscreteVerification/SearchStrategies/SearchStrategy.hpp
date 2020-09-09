/*
 * SearchStrategy.hpp
 *
 *  Created on: 10/09/2012
 *      Author: MathiasGS
 */

#ifndef SEARCHSTRATEGY_HPP_
#define SEARCHSTRATEGY_HPP_

#include "../DataStructures/WaitingList.hpp"

namespace VerifyTAPN::DiscreteVerification {

    template<class T>
    class SearchStrategy {
    public:
        virtual ~SearchStrategy() = default;;

        virtual WaitingList<T> *createWaitingList(AST::Query *query) const = 0;
    };

}

#endif /* SEARCHSTRATEGY_HPP_ */
