/*
 * DeadlockVisitor.cpp
 *
 *  Created on: 01/07/2013
 *      Author: Peter Gjoel Jensen
 */

#include "DiscreteVerification/DeadlockVisitor.hpp"

namespace VerifyTAPN {
    namespace DiscreteVerification {

        using namespace AST;

        void DeadlockVisitor::visit(NotExpression &expr, Result &context) {
            expr.getChild().accept(*this, context);
        }

        void DeadlockVisitor::visit(OrExpression &expr, Result &context) {
            BoolResult left, right;
            expr.getLeft().accept(*this, left);
            expr.getRight().accept(*this, right);

            static_cast<BoolResult &>(context).value
                    = left.value || right.value;
        }

        void DeadlockVisitor::visit(AndExpression &expr, Result &context) {
            BoolResult left, right;
            expr.getLeft().accept(*this, left);
            expr.getRight().accept(*this, right);

            static_cast<BoolResult &>(context).value
                    = left.value || right.value;
        }

        void DeadlockVisitor::visit(AtomicProposition &expr, Result &context) {
            static_cast<BoolResult &>(context).value = false;
        }

        void DeadlockVisitor::visit(BoolExpression &expr, Result &context) {
            static_cast<BoolResult &>(context).value = false;
        }

        void DeadlockVisitor::visit(Query &query, Result &context) {
            query.getChild()->accept(*this, context);
        }

        void DeadlockVisitor::visit(DeadlockExpression &expr, Result &context) {
            static_cast<BoolResult &>(context).value = true;
        }

        void DeadlockVisitor::visit(NumberExpression &expr, Result &context) {
            static_cast<BoolResult &>(context).value = false;
        }

        void DeadlockVisitor::visit(IdentifierExpression &expr, Result &context) {
            static_cast<BoolResult &>(context).value = false;
        }

        void DeadlockVisitor::visit(MultiplyExpression &expr, Result &context) {
            static_cast<BoolResult &>(context).value = false;
        }

        void DeadlockVisitor::visit(MinusExpression &expr, Result &context) {
            static_cast<BoolResult &>(context).value = false;
        }

        void DeadlockVisitor::visit(SubtractExpression &expr, Result &context) {
            static_cast<BoolResult &>(context).value = false;
        }

        void DeadlockVisitor::visit(PlusExpression &expr, Result &context) {
            static_cast<BoolResult &>(context).value = false;
        }
    }
}
