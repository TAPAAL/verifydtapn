/*
 * PlaceVisitor.hpp
 *
 *  Created on: 21/03/2012
 *      Author: MathiasGS
 */

#ifndef PLACEVISITOR_HPP_
#define PLACEVISITOR_HPP_

#include "../Core/QueryParser/Visitor.hpp"
#include "../Core/QueryParser/AST.hpp"
#include <exception>
#include <vector>

namespace VerifyTAPN::DiscreteVerification {

        using namespace AST;

        class PlaceVisitor : public Visitor {
        public:
            PlaceVisitor() = default;;

            ~PlaceVisitor() override = default;;

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

            void visit(MinusExpression &expr, Result &context) override;

            virtual void visit(OperationExpression &expr, Result &context);

            void visit(MultiplyExpression &expr, Result &context) override {
                visit((OperationExpression &) expr, context);
            };

            void visit(SubtractExpression &expr, Result &context) override {
                visit((OperationExpression &) expr, context);
            };;

            void visit(PlusExpression &expr, Result &context) override {
                visit((OperationExpression &) expr, context);
            };;
        };

    } /* namespace VerifyTAPN */
#endif /* PLACEVISITOR_HPP_ */
