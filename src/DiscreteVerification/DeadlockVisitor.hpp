/*
 * DeadlockVisitor.hpp
 *
 *  Created on: 01/07/2013
 *      Author: Peter Gjoel Jensen
 */

#ifndef DEADLOCKVISITOR_HPP_
#define DEADLOCKVISITOR_HPP_

#include "../Core/QueryParser/Visitor.hpp"
#include "DataStructures/NonStrictMarking.hpp"
#include "../Core/QueryParser/AST.hpp"
#include <exception>

namespace VerifyTAPN {
    namespace DiscreteVerification {

        using namespace AST;

        class DeadlockVisitor : public Visitor {
        public:
            
            DeadlockVisitor() {
            };

            virtual ~DeadlockVisitor() {
            };

        public: // visitor methods

            virtual void visit(const NotExpression& expr, boost::any& context);
            virtual void visit(const ParExpression& expr, boost::any& context);
            virtual void visit(const OrExpression& expr, boost::any& context);
            virtual void visit(const AndExpression& expr, boost::any& context);
            virtual void visit(const AtomicProposition& expr, boost::any& context);
            virtual void visit(const BoolExpression& expr, boost::any& context);
            virtual void visit(const Query& query, boost::any& context);
            virtual void visit(const DeadlockExpression& expr, boost::any& context);

        };
    } /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* DEADLOCKVISITOR_HPP_ */
