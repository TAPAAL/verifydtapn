/*
 * DeadlockVisitor.hpp
 *
 *  Created on: 21/03/2012
 *      Author: Peter Gjoel Jensen
 */

#ifndef DEADLOCKVISITOR_HPP_
#define DEADLOCKVISITOR_HPP_

#include "../Core/QueryParser/Visitor.hpp"
#include "DataStructures/NonStrictMarking.hpp"
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

            virtual void visit(const NotExpression& expr, boost::any& context);
            virtual void visit(const ParExpression& expr, boost::any& context);
            virtual void visit(const OrExpression& expr, boost::any& context);
            virtual void visit(const AndExpression& expr, boost::any& context);
            virtual void visit(const AtomicProposition& expr, boost::any& context);
            virtual void visit(const BoolExpression& expr, boost::any& context);
            virtual void visit(const Query& query, boost::any& context);
            virtual void visit(const DeadlockExpression& expr, boost::any& context);

        };
        
        void DeadlockVisitor::visit(const NotExpression& expr, boost::any& context) {
            expr.getChild().accept(*this, context);
        }

        void DeadlockVisitor::visit(const ParExpression& expr, boost::any& context) {
            expr.getChild().accept(*this, context);
        }

        void DeadlockVisitor::visit(const OrExpression& expr, boost::any& context) {
            boost::any left, right;
            expr.getLeft().accept(*this, left);
            expr.getRight().accept(*this, right);

            context = boost::any_cast<bool>(left) || boost::any_cast<bool>(right);
        }

        void DeadlockVisitor::visit(const AndExpression& expr, boost::any& context) {
            boost::any left, right;
            expr.getLeft().accept(*this, left);
            expr.getRight().accept(*this, right);

            context = boost::any_cast<bool>(left) || boost::any_cast<bool>(right);
        }

        void DeadlockVisitor::visit(const AtomicProposition& expr, boost::any& context) {
            context = false;
        }

        void DeadlockVisitor::visit(const BoolExpression& expr, boost::any& context) {
            context = false;
        }

        void DeadlockVisitor::visit(const Query& query, boost::any& context) {
            query.getChild().accept(*this, context);
        }
        
        void DeadlockVisitor::visit(const DeadlockExpression& expr, boost::any& context) {
            context = true;
        }
    } /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* DEADLOCKVISITOR_HPP_ */
