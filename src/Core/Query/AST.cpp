#include "Core/Query/AST.hpp"
#include <cassert>

namespace VerifyTAPN {
    namespace AST {

        NotExpression *NotExpression::clone() const {
            return new NotExpression(*this);
        }

        void NotExpression::accept(Visitor &visitor, Result &context) {
            visitor.visit(*this, context);
        }

        void BoolExpression::accept(Visitor &visitor, Result &context) {
            visitor.visit(*this, context);
        }

        BoolExpression *BoolExpression::clone() const {
            return new BoolExpression(*this);
        }

        void DeadlockExpression::accept(Visitor &visitor, Result &context) {
            visitor.visit(*this, context);
        }

        DeadlockExpression *DeadlockExpression::clone() const {
            return new DeadlockExpression(*this);
        }

        void AtomicProposition::accept(Visitor &visitor, Result &context) {
            visitor.visit(*this, context);
        }

        AtomicProposition *AtomicProposition::clone() const {
            return new AtomicProposition(*this);
        }

        AndExpression *AndExpression::clone() const {
            return new AndExpression(*this);
        }

        void AndExpression::accept(Visitor &visitor, Result &context) {
            visitor.visit(*this, context);
        }

        OrExpression *OrExpression::clone() const {
            return new OrExpression(*this);
        }

        void OrExpression::accept(Visitor &visitor, Result &context) {
            visitor.visit(*this, context);
        }

        void PlusExpression::accept(Visitor &visitor, Result &context) {
            visitor.visit(*this, context);
        }

        PlusExpression *PlusExpression::clone() const {
            return new PlusExpression(*this);
        }

        void SubtractExpression::accept(Visitor &visitor, Result &context) {
            visitor.visit(*this, context);
        }

        SubtractExpression *SubtractExpression::clone() const {
            return new SubtractExpression(*this);
        }

        void MinusExpression::accept(Visitor &visitor, Result &context) {
            visitor.visit(*this, context);
        }

        MinusExpression *MinusExpression::clone() const {
            return new MinusExpression(*this);
        }

        void MultiplyExpression::accept(Visitor &visitor, Result &context) {
            visitor.visit(*this, context);
        }

        MultiplyExpression *MultiplyExpression::clone() const {
            return new MultiplyExpression(*this);
        }

        void NumberExpression::accept(Visitor &visitor, Result &context) {
            visitor.visit(*this, context);
        }

        NumberExpression *NumberExpression::clone() const {
            return new NumberExpression(*this);
        }

        void IdentifierExpression::accept(Visitor &visitor, Result &context) {
            visitor.visit(*this, context);
        }

        IdentifierExpression *IdentifierExpression::clone() const {
            return new IdentifierExpression(*this);
        }

        Query *Query::clone() const {
            return new Query(*this);
        }

        void Query::accept(Visitor &visitor, Result &context) {
            visitor.visit(*this, context);
        }

        AtomicProposition::AtomicProposition(ArithmeticExpression *l, std::string *sop, ArithmeticExpression *r) : left(l), right(r) {
            if (*sop == "=" || *sop == "==") op = EQ;
            else if (*sop == "!=") op = NE;
            else if (*sop == "<") op = LT;
            else if (*sop == "<=") op = LE;
            else if (*sop == ">=") {
                op = LE;
                std::swap(left, right);
            } else if (*sop == ">") {
                op = LT;
                std::swap(left, right);
            } else {
                assert(false);
                throw std::exception();
            }
        }

        Query* toAST(const unfoldtacpn::PQL::Condition_ptr& ptr) {
            return nullptr;
        }
    }
}
