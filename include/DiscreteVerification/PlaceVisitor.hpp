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

namespace VerifyTAPN {
    namespace DiscreteVerification {

        using namespace AST;

        class PlaceVisitor : public Visitor {
        public:
            PlaceVisitor() {};

            virtual ~PlaceVisitor() {};

        public: // visitor methods
            virtual void visit(NotExpression &expr, Result &context);

            virtual void visit(OrExpression &expr, Result &context);

            virtual void visit(AndExpression &expr, Result &context);

            virtual void visit(AtomicProposition &expr, Result &context);

            virtual void visit(BoolExpression &expr, Result &context);

            virtual void visit(Query &query, Result &context);

            virtual void visit(DeadlockExpression &expr, Result &context);

            virtual void visit(NumberExpression &expr, Result &context);

            virtual void visit(IdentifierExpression &expr, Result &context);

            virtual void visit(MinusExpression &expr, Result &context);

            virtual void visit(OperationExpression &expr, Result &context);

            virtual void visit(MultiplyExpression &expr, Result &context) {
                visit((OperationExpression &) expr, context);
            };

            virtual void visit(SubtractExpression &expr, Result &context) {
                visit((OperationExpression &) expr, context);
            };;

            virtual void visit(PlusExpression &expr, Result &context) {
                visit((OperationExpression &) expr, context);
            };;
        };

    } /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* PLACEVISITOR_HPP_ */
