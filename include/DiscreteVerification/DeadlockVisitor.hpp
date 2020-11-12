/*
 * DeadlockVisitor.hpp
 *
 *  Created on: 01/07/2013
 *      Author: Peter Gjoel Jensen
 */

#ifndef DEADLOCKVISITOR_HPP_
#define DEADLOCKVISITOR_HPP_

#include "Core/QueryParser/Visitor.hpp"
#include "Core/QueryParser/AST.hpp"
#include <exception>

namespace VerifyTAPN::DiscreteVerification {

    using namespace AST;

    class DeadlockVisitor : public Visitor {
    public:

        DeadlockVisitor() = default;

        ~DeadlockVisitor() override = default;

    public: // visitor methods
        void visit(NotExpression &expr, Result &context) override;

        void visit(OrExpression &expr, Result &context) override;

        void visit(AndExpression &expr, Result &context) override;

        void visit(AtomicProposition &expr, Result &context) override;

        void visit(BoolExpression &expr, Result &context) override;

        void visit(Query &query, Result &context) override;

        void visit(DeadlockExpression &expr, Result &context) override;

        void visit(NumberExpression &expr, Result &context) override;

        void visit(IdentifierExpression &expr, Result &context) override;

        void visit(MultiplyExpression &expr, Result &context) override;

        void visit(MinusExpression &expr, Result &context) override;

        void visit(SubtractExpression &expr, Result &context) override;

        void visit(PlusExpression &expr, Result &context) override;

    };
} /* namespace VerifyTAPN */
#endif /* DEADLOCKVISITOR_HPP_ */
