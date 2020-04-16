/*
 * NonStrictDFS.hpp
 *
 *  Created on: 05/03/2012
 *      Author: MathiasGS
 */

#ifndef NONSTRICTDFSHEURISTIC_HPP_
#define NONSTRICTDFSHEURISTIC_HPP_

#include "../DataStructures/PWList.hpp"
#include "../../Core/TAPN/TAPN.hpp"
#include "../../Core/QueryParser/AST.hpp"
#include "../../Core/VerificationOptions.hpp"

#include "../../Core/TAPN/TimedPlace.hpp"
#include "../../Core/TAPN/TimedTransition.hpp"
#include "../../Core/TAPN/TimedInputArc.hpp"
#include "../../Core/TAPN/TransportArc.hpp"
#include "../../Core/TAPN/InhibitorArc.hpp"
#include "../../Core/TAPN/OutputArc.hpp"

#include "../QueryVisitor.hpp"

#include <stack>
#include "SearchStrategy.hpp"

namespace VerifyTAPN::DiscreteVerification {

    template<class T>
    class NonStrictDFSHeuristic : public SearchStrategy<T> {
    public:
        virtual WaitingList <T> *
        createWaitingList(AST::Query *query) const { return new HeuristicStackWaitingList<T>(query); };
    };

}
#endif /* NONSTRICTDFS_HPP_ */
