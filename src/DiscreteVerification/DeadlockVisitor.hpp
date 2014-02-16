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
            virtual void visit(const NotExpression& expr, Result& context);
            virtual void visit(const OrExpression& expr, Result& context);
            virtual void visit(const AndExpression& expr, Result& context);
            virtual void visit(const AtomicProposition& expr, Result& context);
            virtual void visit(const BoolExpression& expr, Result& context);
            virtual void visit(const Query& query, Result& context);
            virtual void visit(const DeadlockExpression& expr, Result& context);
            virtual void visit(const NumberExpression& expr, Result& context);
            virtual void visit(const IdentifierExpression& expr, Result& context);
            virtual void visit(const MultiplyExpression& expr, Result& context);
            virtual void visit(const MinusExpression& expr, Result& context);
            virtual void visit(const SubtractExpression& expr, Result& context);
            virtual void visit(const PlusExpression& expr, Result& context);

        };
    } /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* DEADLOCKVISITOR_HPP_ */
