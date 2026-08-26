/*
 * QueryVisitor.hpp
 *
 *  Created on: 21/03/2012
 *      Author: MathiasGS
 */

#ifndef QUERYVISITOR_HPP_
#define QUERYVISITOR_HPP_

#include "Core/Query/Visitor.hpp"
#include "DataStructures/NonStrictMarking.hpp"
#include "Core/Query/AST.hpp"

#include <exception>
#include <iostream>
#include <cassert>
#include <variant>

namespace VerifyTAPN { namespace DiscreteVerification {

    using namespace AST;

    template<typename T>
    class QueryVisitor : public Visitor {
    public:

        QueryVisitor(T &marking, const TAPN::TimedArcPetriNet &tapn, int maxDelay) : marking(marking), tapn(tapn),
                                                                                     maxDelay(maxDelay) {
            deadlockChecked = false;
            deadlocked = false;
        };

        QueryVisitor(T &marking, const TAPN::TimedArcPetriNet &tapn) : marking(marking), tapn(tapn), maxDelay(0) {
            deadlockChecked = false;
            deadlocked = false;
        }

        ~QueryVisitor() override = default;

    public: // visitor methods

        void visit(NotExpression &expr, AST::Result &context) override;

        void visit(OrExpression &expr, AST::Result &context) override;

        void visit(AndExpression &expr, AST::Result &context) override;

        void visit(AtomicProposition &expr, AST::Result &context) override;

        void visit(BoolExpression &expr, AST::Result &context) override;

        void visit(Query &query, AST::Result &context) override;

        void visit(DeadlockExpression &expr, AST::Result &context) override;

        void visit(IntExpression &expr, AST::Result &context) override;

        void visit(RealExpression &expr, AST::Result &context) override;

        void visit(IdentifierExpression &expr, AST::Result &context) override;

        void visit(MultiplyExpression &expr, AST::Result &context) override;

        void visit(MinusExpression &expr, AST::Result &context) override;

        void visit(SubtractExpression &expr, AST::Result &context) override;

        void visit(PlusExpression &expr, AST::Result &context) override;

    private:
        bool compare(double leftVal, AtomicProposition::op_e op, double rightVal) const;

    private:
        const T &marking;
        const TAPN::TimedArcPetriNet &tapn;
        bool deadlockChecked;
        bool deadlocked;
        const int maxDelay;
    };

    template<typename T>
    void QueryVisitor<T>::visit(NotExpression &expr, AST::Result &context) {
        BoolResult c;
        expr.getChild().accept(*this, c);
        static_cast<BoolResult &>(context).value = !c.value;
    }

    template<typename T>
    void QueryVisitor<T>::visit(OrExpression &expr, AST::Result &context) {
        BoolResult left, right;
        expr.getLeft().accept(*this, left);
        // use lazy evaluation
        if (left.value) {
            static_cast<BoolResult &>(context).value = true;
        } else {
            expr.getRight().accept(*this, right);
            static_cast<BoolResult &>(context).value = right.value;
        }
    }

    template<typename T>
    void QueryVisitor<T>::visit(AndExpression &expr, AST::Result &context) {
        BoolResult left, right;
        expr.getLeft().accept(*this, left);

        // use lazy evaluation
        if (!left.value) {
            static_cast<BoolResult &>(context).value = false;
        } else {
            expr.getRight().accept(*this, right);
            static_cast<BoolResult &>(context).value = right.value;
        }
    }

    template<typename T>
    void QueryVisitor<T>::visit(AtomicProposition &expr, AST::Result &context) {
        NumberResult left, right;
        expr.getLeft().accept(*this, left);
        expr.getRight().accept(*this, right);

        static_cast<BoolResult &>(context).value = compare(left.value, expr.getOperator(), right.value);
    }

    template<typename T>
    void QueryVisitor<T>::visit(BoolExpression &expr, AST::Result &context) {
        static_cast<BoolResult &>(context).value = expr.getValue();
    }

    template<typename T>
    void QueryVisitor<T>::visit(IntExpression &expr, AST::Result &context) {
        static_cast<NumberResult &>(context).value = static_cast<double>(expr.getValue());
    }

    template<typename T>
    void QueryVisitor<T>::visit(RealExpression &expr, AST::Result &context) {
        static_cast<NumberResult &>(context).value = static_cast<double>(expr.getValue());
    }

    template<typename T>
    void QueryVisitor<T>::visit(IdentifierExpression &expr, AST::Result &context) {
        int tokens = marking.numberOfTokensInPlace(expr.getPlace());
        static_cast<NumberResult &>(context).value = static_cast<double>(tokens);
    }

    template<typename T>
    void QueryVisitor<T>::visit(MultiplyExpression &expr, AST::Result &context) {
        NumberResult left, right;
        expr.getLeft().accept(*this, left);
        expr.getRight().accept(*this, right);
        static_cast<NumberResult &>(context).value = left.value * right.value;
    }

    template<typename T>
    void QueryVisitor<T>::visit(MinusExpression &expr, AST::Result &context) {
        NumberResult val;
        expr.getValue().accept(*this, val);
        static_cast<NumberResult &>(context).value = -val.value;
    }

    template<typename T>
    void QueryVisitor<T>::visit(SubtractExpression &expr, AST::Result &context) {
        NumberResult left, right;
        expr.getLeft().accept(*this, left);
        expr.getRight().accept(*this, right);
        static_cast<NumberResult &>(context).value = left.value - right.value;
    }

    template<typename T>
    void QueryVisitor<T>::visit(PlusExpression &expr, AST::Result &context) {
        NumberResult left, right;
        expr.getLeft().accept(*this, left);
        expr.getRight().accept(*this, right);
        static_cast<NumberResult &>(context).value = left.value + right.value;
    }

    template<typename T>
    void QueryVisitor<T>::visit(Query &query, AST::Result &context) {
        query.getChild()->accept(*this, context);
        if (query.getQuantifier() == AG || query.getQuantifier() == AF || query.getQuantifier() == PG) {
            static_cast<BoolResult &>(context).value = !static_cast<BoolResult &>(context).value;
        }
    }

    template<typename T>
    void QueryVisitor<T>::visit(DeadlockExpression &expr, AST::Result &context) {
        if (!deadlockChecked) {
            deadlockChecked = true;
            deadlocked = marking.canDeadlock(tapn, maxDelay);
        }
        static_cast<BoolResult &>(context).value = deadlocked;
    }

    template<typename T>
    bool QueryVisitor<T>::compare(double leftVal, AtomicProposition::op_e op, double rightVal) const {
        switch(op) {
            case AtomicProposition::LT: return leftVal < rightVal;
            case AtomicProposition::LE: return leftVal <= rightVal;
            case AtomicProposition::EQ: return leftVal == rightVal;
            case AtomicProposition::NE: return leftVal != rightVal;
            default: assert(false);
        }
        return false;
    }

} } /* namespace VerifyTAPN */
#endif /* QUERYVISITOR_HPP_ */