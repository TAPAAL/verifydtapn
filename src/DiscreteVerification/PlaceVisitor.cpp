/*
 * PlaceVisitor.cpp
 *
 *  Created on: 21/03/2012
 *      Author: MathiasGS
 */

#include "DiscreteVerification/PlaceVisitor.hpp"

namespace VerifyTAPN::DiscreteVerification {

        void PlaceVisitor::visit(NotExpression &expr, Result &context) {
            expr.getChild().accept(*this, context);
        }

        void PlaceVisitor::visit(OrExpression &expr, Result &context) {
            expr.getLeft().accept(*this, context);
            expr.getRight().accept(*this, context);
        }

        void PlaceVisitor::visit(AndExpression &expr, Result &context) {
            expr.getLeft().accept(*this, context);
            expr.getRight().accept(*this, context);
        }

        void PlaceVisitor::visit(AtomicProposition &expr, Result &context) {
            expr.getLeft().accept(*this, context);
            expr.getRight().accept(*this, context);
        }

        void PlaceVisitor::visit(DeadlockExpression &expr, Result &context) {
        }

        void PlaceVisitor::visit(BoolExpression &expr, Result &context) {
        }

        void PlaceVisitor::visit(Query &query, Result &context) {
            query.getChild()->accept(*this, context);
        }

        void PlaceVisitor::visit(NumberExpression &expr, Result &context) {};

        void PlaceVisitor::visit(IdentifierExpression &expr, Result &context) {
            auto &v = static_cast< AST::IntVectorResult & >(context);
            v.value.push_back(expr.getPlace());
        };

        void PlaceVisitor::visit(MinusExpression &expr, Result &context) {
            expr.getValue().accept(*this, context);
        };

        void PlaceVisitor::visit(OperationExpression &expr, Result &context) {
            expr.getLeft().accept(*this, context);
            expr.getRight().accept(*this, context);
        };
    } /* namespace VerifyTAPN */
