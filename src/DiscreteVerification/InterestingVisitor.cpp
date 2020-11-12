
#include "DiscreteVerification/InterestingVisitor.h"
#include "DiscreteVerification/ReducingGenerator.hpp"

namespace VerifyTAPN::DiscreteVerification {
    void InterestingVisitor::clear() {
        std::fill(_decr.begin(), _decr.end(), 0);
        std::fill(_incr.begin(), _incr.end(), 0);
        deadlock = false;
    }

    void InterestingVisitor::visit(NotExpression &expr, Result &context) {
        negate();
        expr.getChild().accept(*this, context);
        negate();
    }

    void InterestingVisitor::visit(OrExpression &expr, Result &context) {
        if (!negated) {
            expr.getLeft().accept(*this, context);
            expr.getRight().accept(*this, context);
        } else {
            if (expr.getLeft().eval)
                expr.getLeft().accept(*this, context);
            else if (expr.getRight().eval)
                expr.getRight().accept(*this, context);
        }
    }

    void InterestingVisitor::visit(AndExpression &expr, Result &context) {
        if (negated) {
            expr.getLeft().accept(*this, context);
            expr.getRight().accept(*this, context);
        } else {
            if (!expr.getLeft().eval)
                expr.getLeft().accept(*this, context);
            else if (!expr.getRight().eval)
                expr.getRight().accept(*this, context);
        }
    }

    void InterestingVisitor::visit(AtomicProposition &expr, Result &context) {
        auto incdec = [this, &expr](bool id1, bool id2) {
            IncDecr ic(true, false);
            IncDecr dc(false, true);
            if (id1) expr.getLeft().accept(*this, ic);
            else expr.getLeft().accept(*this, dc);
            if (id2) expr.getRight().accept(*this, ic);
            else expr.getRight().accept(*this, dc);
        };
        switch(expr.getOperator())
        {
            case AtomicProposition::LT:
            case AtomicProposition::LE:
                if (!expr.eval && !negated)
                    incdec(false, true);
                else if (expr.eval && negated)
                    incdec(true, false);
                break;
            case AtomicProposition::EQ:
            case AtomicProposition::NE:
            {
                bool neg = negated == (expr.getOperator() == AtomicProposition::EQ);
                if (!expr.eval && neg) {
                    if (expr.getLeft().eval < expr.getRight().eval)
                        incdec(true, false);
                    else
                        incdec(false, true);
                } else if (expr.eval && neg) {
                    incdec(true, true);
                    incdec(false, false);
                }
                break;
            }
            default: assert(false);
        }
    }

    void InterestingVisitor::visit(BoolExpression &expr, Result &context) {
        // nothing!
    }

    void InterestingVisitor::visit(Query &query, Result &context) {
        if (query.getQuantifier() == Quantifier::AG) negate();
        query.getChild()->accept(*this, context);
        if (query.getQuantifier() == Quantifier::AG) negate();
    }

    void InterestingVisitor::visit(DeadlockExpression &expr, Result &context) {
        if (!negated) deadlock = !negated;
    }

    void InterestingVisitor::visit(NumberExpression &expr, Result &context) {
        // nothing!
    }

    void InterestingVisitor::visit(IdentifierExpression &expr, Result &context) {
        auto &id = static_cast<IncDecr &> (context);
        if (id.incr) _incr[expr.getPlace()] = true;
        if (id.decr) _decr[expr.getPlace()] = true;
    }

    void InterestingVisitor::visit(MultiplyExpression &expr, Result &context) {
        IncDecr c(true, true);
        expr.getLeft().accept(*this, c);
        expr.getRight().accept(*this, c);
    }

    void InterestingVisitor::visit(MinusExpression &expr, Result &context) {
        auto &id = static_cast<IncDecr &> (context);
        IncDecr down(false, true);
        IncDecr up(true, false);
        if (id.incr && id.decr) expr.accept(*this, id);
        else if (id.incr) expr.accept(*this, down);
        else if (id.decr) expr.accept(*this, up);
    }

    void InterestingVisitor::visit(SubtractExpression &expr, Result &context) {
        auto &id = static_cast<IncDecr &> (context);
        IncDecr down(false, true);
        IncDecr up(true, false);
        if (id.incr && id.decr)
        {
            expr.accept(*this, id);
        } else if (id.incr) {
            expr.getLeft().accept(*this, up);
            expr.getRight().accept(*this, down);
        } else if (id.decr) {
            expr.getLeft().accept(*this, down);
            expr.getRight().accept(*this, up);
        }
    }

    void InterestingVisitor::visit(PlusExpression &expr, Result &context) {
        expr.getLeft().accept(*this, context);
        expr.getRight().accept(*this, context);
    }
}