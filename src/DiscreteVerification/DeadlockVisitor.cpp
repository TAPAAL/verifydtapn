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

        void DeadlockVisitor::visit(const NotExpression& expr, Result& context) {
            expr.getChild().accept(*this, context);
        }

        void DeadlockVisitor::visit(const ParExpression& expr, Result& context) {
            expr.getChild().accept(*this, context);
        }

        void DeadlockVisitor::visit(const OrExpression& expr, Result& context) {
            BoolResult left, right;
            expr.getLeft().accept(*this, left);
            expr.getRight().accept(*this, right);

            static_cast<BoolResult&>(context).value
                    = left.value || right.value;
        }

        void DeadlockVisitor::visit(const AndExpression& expr, Result& context) {
            BoolResult left, right;
            expr.getLeft().accept(*this, left);
            expr.getRight().accept(*this, right);

            static_cast<BoolResult&>(context).value 
                    = left.value || right.value;
        }

        void DeadlockVisitor::visit(const AtomicProposition& expr, Result& context) {
            static_cast<BoolResult&>(context).value = false;
        }

        void DeadlockVisitor::visit(const BoolExpression& expr, Result& context) {
            static_cast<BoolResult&>(context).value = false;
        }

        void DeadlockVisitor::visit(const Query& query, Result& context) {
            query.getChild().accept(*this, context);
        }

        void DeadlockVisitor::visit(const DeadlockExpression& expr, Result& context) {
            static_cast<BoolResult&>(context).value = true;
        }
    }
}
