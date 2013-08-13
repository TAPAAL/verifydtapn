/*
 * DeadlockVisitor.cpp
 *
 *  Created on: 01/07/2013
 *      Author: Peter Gjoel Jensen
 */

#include "DeadlockVisitor.hpp"

namespace VerifyTAPN {
    namespace DiscreteVerification {

        using namespace AST;

        void DeadlockVisitor::visit(const NotExpression& expr, boost::any& context) {
            expr.getChild().accept(*this, context);
        }

        void DeadlockVisitor::visit(const ParExpression& expr, boost::any& context) {
            expr.getChild().accept(*this, context);
        }

        void DeadlockVisitor::visit(const OrExpression& expr, boost::any& context) {
            boost::any left, right;
            expr.getLeft().accept(*this, left);
            expr.getRight().accept(*this, right);

            context = boost::any_cast<bool>(left) || boost::any_cast<bool>(right);
        }

        void DeadlockVisitor::visit(const AndExpression& expr, boost::any& context) {
            boost::any left, right;
            expr.getLeft().accept(*this, left);
            expr.getRight().accept(*this, right);

            context = boost::any_cast<bool>(left) || boost::any_cast<bool>(right);
        }

        void DeadlockVisitor::visit(const AtomicProposition& expr, boost::any& context) {
            context = false;
        }

        void DeadlockVisitor::visit(const BoolExpression& expr, boost::any& context) {
            context = false;
        }

        void DeadlockVisitor::visit(const Query& query, boost::any& context) {
            query.getChild().accept(*this, context);
        }

        void DeadlockVisitor::visit(const DeadlockExpression& expr, boost::any& context) {
            context = true;
        }
    }
}
