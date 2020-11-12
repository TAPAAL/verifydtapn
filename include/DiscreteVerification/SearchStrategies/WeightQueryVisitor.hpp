/*
 * QueryVisitor.hpp
 *
 *  Created on: 21/03/2012
 *      Author: MathiasGS
 */

#ifndef WEIGHTQUERYVISITOR_HPP_
#define WEIGHTQUERYVISITOR_HPP_

#include "Core/QueryParser/Visitor.hpp"
#include "../DataStructures/NonStrictMarking.hpp"
#include "Core/QueryParser/AST.hpp"
#include <exception>
#include <climits>

namespace VerifyTAPN::DiscreteVerification {

    using namespace AST;

    class WeightQueryVisitor : public Visitor {
    public:
        explicit WeightQueryVisitor(NonStrictMarkingBase &marking) : marking(marking) {};

        ~WeightQueryVisitor() override = default;;

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

    private:
        static int compare(int numberOfTokensInPlace, AtomicProposition::op_e op, int n);

    private:
        const NonStrictMarkingBase &marking;
    };

} /* namespace VerifyTAPN */
#endif /* QUERYVISITOR_HPP_ */
