/*
 * NonStrictDFS.hpp
 *
 *  Created on: 05/03/2012
 *      Author: MathiasGS
 */

#ifndef NONSTRICTHEURISTIC_HPP_
#define NONSTRICTHEURISTIC_HPP_

#include "DiscreteVerification/DataStructures/PWList.hpp"
#include "Core/TAPN/TAPN.hpp"
#include "Core/Query/AST.hpp"
#include "Core/VerificationOptions.hpp"

#include "Core/TAPN/TimedPlace.hpp"
#include "Core/TAPN/TimedTransition.hpp"
#include "Core/TAPN/TimedInputArc.hpp"
#include "Core/TAPN/TransportArc.hpp"
#include "Core/TAPN/InhibitorArc.hpp"
#include "Core/TAPN/OutputArc.hpp"

#include "DiscreteVerification/QueryVisitor.hpp"

#include <stack>
#include "SearchStrategy.hpp"

namespace VerifyTAPN { namespace DiscreteVerification {

    template<class T>
    class NonStrictHeuristic : public SearchStrategy<T> {
    public:
        virtual WaitingList <T> *createWaitingList(AST::Query *query) const {
            return new HeuristicWaitingList<T>(query);
        };
    };
} }
#endif /* NONSTRICTDFS_HPP_ */
