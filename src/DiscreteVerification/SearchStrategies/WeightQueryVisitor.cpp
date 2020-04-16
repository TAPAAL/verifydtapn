/*
 * QueryVisitor.cpp
 *
 *  Created on: 21/03/2012
 *      Author: MathiasGS
 */

#include "DiscreteVerification/SearchStrategies/WeightQueryVisitor.hpp"

namespace VerifyTAPN::DiscreteVerification {

    void WeightQueryVisitor::visit(NotExpression &expr, Result &context) {
        assert(false);
    }

    void WeightQueryVisitor::visit(OrExpression &expr, Result &context) {
        IntResult left, right;
        expr.getLeft().accept(*this, left);
        expr.getRight().accept(*this, right);

        static_cast<IntResult &>(context).value
                = min(left.value, right.value);
    }

    void WeightQueryVisitor::visit(AndExpression &expr, Result &context) {
        IntResult left, right;

        expr.getLeft().accept(*this, left);
        if (left.value == std::numeric_limits<int>::max()) {
            static_cast<IntResult &>(context).value = left.value;
            return;
        }

        expr.getRight().accept(*this, right);
        if (right.value == std::numeric_limits<int>::max()) {
            static_cast<IntResult &>(context).value = right.value;
            return;
        }

        static_cast<IntResult &>(context).value
                = left.value + right.value;
    }

    void WeightQueryVisitor::visit(DeadlockExpression &expr, Result &context) {
        static_cast<IntResult &>(context).value = 0;
    }

    void WeightQueryVisitor::visit(AtomicProposition &expr, Result &context) {
        IntResult left;
        expr.getLeft().accept(*this, left);
        IntResult right;
        expr.getRight().accept(*this, right);
        static_cast<IntResult &>(context).value
                = compare(left.value, expr.getOperator(), right.value);
    }

    void WeightQueryVisitor::visit(NumberExpression &expr, Result &context) {
        ((IntResult &) context).value = expr.getValue();
    }

    void WeightQueryVisitor::visit(IdentifierExpression &expr, Result &context) {
        ((IntResult &) context).value = marking.numberOfTokensInPlace(expr.getPlace());
    }

    void WeightQueryVisitor::visit(MultiplyExpression &expr, Result &context) {
        IntResult left;
        expr.getLeft().accept(*this, left);
        IntResult right;
        expr.getRight().accept(*this, right);
        ((IntResult &) context).value = left.value * right.value;
    }

    void WeightQueryVisitor::visit(MinusExpression &expr, Result &context) {
        IntResult value;
        expr.getValue().accept(*this, value);
        ((IntResult &) context).value = -value.value;
    }

    void WeightQueryVisitor::visit(SubtractExpression &expr, Result &context) {
        IntResult left;
        expr.getLeft().accept(*this, left);
        IntResult right;
        expr.getRight().accept(*this, right);
        ((IntResult &) context).value = left.value - right.value;
    }

    void WeightQueryVisitor::visit(PlusExpression &expr, Result &context) {
        IntResult left;
        expr.getLeft().accept(*this, left);
        IntResult right;
        expr.getRight().accept(*this, right);
        ((IntResult &) context).value = left.value + right.value;
    }

    void WeightQueryVisitor::visit(BoolExpression &expr, Result &context) {
        static_cast<IntResult &>(context).value
                = expr.getValue() ? 0 : std::numeric_limits<int>::max();
    }

    void WeightQueryVisitor::visit(Query &query, Result &context) {
        query.getChild()->accept(*this, context);
    }

    int WeightQueryVisitor::compare(int numberOfTokensInPlace, const std::string &op, int n) const {
        if (op == "<") return numberOfTokensInPlace < n ? 0 : abs(n - numberOfTokensInPlace);
        else if (op == "<=") return numberOfTokensInPlace <= n ? 0 : abs(n - numberOfTokensInPlace);
        else if (op == "=" || op == "==") return numberOfTokensInPlace == n ? 0 : abs(n - numberOfTokensInPlace);
        else if (op == ">=") return numberOfTokensInPlace >= n ? 0 : abs(n - numberOfTokensInPlace);
        else if (op == ">") return numberOfTokensInPlace > n ? 0 : abs(n - numberOfTokensInPlace);
        else if (op == "!=") return numberOfTokensInPlace == n ? 1 : 0;
        else
            throw std::exception();
    }

} /* namespace VerifyTAPN */
