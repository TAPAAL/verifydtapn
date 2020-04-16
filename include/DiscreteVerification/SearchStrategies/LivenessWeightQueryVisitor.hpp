/*
 * QueryVisitor.hpp
 *
 *  Created on: 21/03/2012
 *      Author: MathiasGS
 */

#ifndef LIVEWEIGHTQUERYVISITOR_HPP_
#define LIVEWEIGHTQUERYVISITOR_HPP_

#include "../../Core/QueryParser/Visitor.hpp"
#include "../DataStructures/NonStrictMarking.hpp"
#include "../../Core/QueryParser/AST.hpp"
#include <exception>
#include <climits>

namespace VerifyTAPN::DiscreteVerification {

        using namespace AST;

        class LivenessWeightQueryVisitor : public Visitor {
        public:
            explicit LivenessWeightQueryVisitor(NonStrictMarkingBase &marking) : marking(marking) {};

            ~LivenessWeightQueryVisitor() override = default;;

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
            int compare(int numberOfTokensInPlace, const std::string &op, int n) const;

        private:
            const NonStrictMarkingBase &marking;
        };

    } /* namespace VerifyTAPN */
#endif /* LIVENESSQUERYVISITOR_HPP_ */
