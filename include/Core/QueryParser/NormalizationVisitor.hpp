/*
 * NormalizationVisitor.hpp
 *
 *  Created on: Nov 20, 2011
 *      Author: mortenja
 */

#ifndef NORMALIZATIONVISITOR_HPP_
#define NORMALIZATIONVISITOR_HPP_

#include "Visitor.hpp"
#include <string>
#include "AST.hpp"

namespace VerifyTAPN {
    namespace AST {
        class Tuple : public Result {
        public:
            bool negate;
            Expression *returnExpr;

            Tuple(bool negate, Expression *expr) : negate(negate), returnExpr(expr) {};
        };

        class NormalizationVisitor : public Visitor {
        public:
            NormalizationVisitor() : normalizedQuery() {};

            ~NormalizationVisitor() override = default;;

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

            AST::Query *normalize(AST::Query &query) {
                Tuple any(false, nullptr);
                query.accept(*this, any);
                return normalizedQuery;
            };
        private:
            std::string negateOperator(const std::string &op) const;

        private:
            AST::Query *normalizedQuery;
        };
    };
}


#endif /* NORMALIZATIONVISITOR_HPP_ */
