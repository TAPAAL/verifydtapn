/*
 * PlaceVisitor.cpp
 *
 *  Created on: 21/03/2012
 *      Author: MathiasGS
 */

#include "PlaceVisitor.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

		void PlaceVisitor::visit(const NotExpression& expr, Result& context)
		{
			expr.getChild().accept(*this, context);
		}

		void PlaceVisitor::visit(const OrExpression& expr, Result& context)
		{
			expr.getLeft().accept(*this, context);
			expr.getRight().accept(*this, context);
		}

		void PlaceVisitor::visit(const AndExpression& expr, Result& context)
		{
			expr.getLeft().accept(*this, context);
			expr.getRight().accept(*this, context);
		}

		void PlaceVisitor::visit(const AtomicProposition& expr, Result& context)
		{
                    expr.getLeft().accept(*this,context);
                    expr.getRight().accept(*this,context);
		}

                void PlaceVisitor::visit(const DeadlockExpression& expr, Result& context)
		{
		}
                
		void PlaceVisitor::visit(const BoolExpression& expr, Result& context)
		{
		}

		void PlaceVisitor::visit(const Query& query, Result& context)
		{
			query.getConstChild().accept(*this, context);
		}
                
                void PlaceVisitor::visit(const NumberExpression& expr, Result& context){};
                
                void PlaceVisitor::visit(const IdentifierExpression& expr, Result& context){
                    AST::IntVectorResult& v = static_cast< AST::IntVectorResult & >(context);
		    v.value.push_back(expr.getPlace());
                };
                
                void PlaceVisitor::visit(const MinusExpression& expr, Result& context){
                    expr.getValue().accept(*this,context);
                };
                
                void PlaceVisitor::visit(const OperationExpression& expr, Result& context){
                    expr.getLeft().accept(*this,context);
                    expr.getRight().accept(*this,context);
                };
} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
