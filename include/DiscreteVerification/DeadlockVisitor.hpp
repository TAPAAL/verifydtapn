/*
 * DeadlockVisitor.hpp
 *
 *  Created on: 01/07/2013
 *      Author: Peter Gjoel Jensen
 */

#ifndef DEADLOCKVISITOR_HPP_
#define DEADLOCKVISITOR_HPP_

#include "../Core/QueryParser/Visitor.hpp"
#include "../Core/QueryParser/AST.hpp"
#include <exception>

namespace VerifyTAPN {
    namespace DiscreteVerification {

        using namespace AST;

        class DeadlockVisitor : public Visitor {
        public:
            
            DeadlockVisitor() {
            };

            virtual ~DeadlockVisitor() {
            };

        public: // visitor methods
            virtual void visit(NotExpression& expr, Result& context);
            virtual void visit(OrExpression& expr, Result& context);
            virtual void visit(AndExpression& expr, Result& context);
            virtual void visit(AtomicProposition& expr, Result& context);
            virtual void visit(BoolExpression& expr, Result& context);
            virtual void visit(Query& query, Result& context);
            virtual void visit(DeadlockExpression& expr, Result& context);
            virtual void visit(NumberExpression& expr, Result& context);
            virtual void visit(IdentifierExpression& expr, Result& context);
            virtual void visit(MultiplyExpression& expr, Result& context);
            virtual void visit(MinusExpression& expr, Result& context);
            virtual void visit(SubtractExpression& expr, Result& context);
            virtual void visit(PlusExpression& expr, Result& context);

        };
    } /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* DEADLOCKVISITOR_HPP_ */
