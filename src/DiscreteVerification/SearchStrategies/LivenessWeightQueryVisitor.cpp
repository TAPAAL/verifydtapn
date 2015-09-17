/*
 * QueryVisitor.cpp
 *
 *  Created on: 21/03/2012
 *      Author: MathiasGS
 */

#include "LivenessWeightQueryVisitor.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

		void LivenessWeightQueryVisitor::visit(const NotExpression& expr, Result& context)
		{
			assert(false);
		}

		void LivenessWeightQueryVisitor::visit(const OrExpression& expr, Result& context)
		{
			IntResult left, right;
			expr.getLeft().accept(*this, left);
			expr.getRight().accept(*this, right);

			static_cast<IntResult&>(context).value 
                                = min(static_cast<IntResult>(left).value, static_cast<IntResult>(right).value);
		}

		void LivenessWeightQueryVisitor::visit(const AndExpression& expr, Result& context)
		{
			IntResult left, right;
                        
			expr.getLeft().accept(*this, left);
			if(left.value == std::numeric_limits<int>::max()){
				static_cast<IntResult&>(context).value = left.value;
				return;
			}
                        
			expr.getRight().accept(*this, right);
			if(right.value == std::numeric_limits<int>::max()){
				static_cast<IntResult&>(context).value = right.value;
				return;
			}

			static_cast<IntResult&>(context).value = left.value+right.value;
		}


		void LivenessWeightQueryVisitor::visit(const AtomicProposition& expr, Result& context)
		{
                    IntResult left;
                    expr.getLeft().accept(*this, left);
                    IntResult right;
                    expr.getLeft().accept(*this, right);
			static_cast<IntResult&>(context).value
                                = compare(left.value, expr.getOperator(), right.value);
		}

                void LivenessWeightQueryVisitor::visit(const NumberExpression& expr, Result& context){
                    ((IntResult&)context).value = expr.getValue();
                }

                void LivenessWeightQueryVisitor::visit(const IdentifierExpression& expr, Result& context){
                    ((IntResult&)context).value = marking.numberOfTokensInPlace(expr.getPlace());
                }
                
                void LivenessWeightQueryVisitor::visit(const MultiplyExpression& expr, Result& context){
                    IntResult left;
                    expr.getLeft().accept(*this, left);
                    IntResult right;
                    expr.getLeft().accept(*this, right);
                    ((IntResult&)context).value = left.value * right.value;
                }
                
                void LivenessWeightQueryVisitor::visit(const MinusExpression& expr, Result& context){
                    IntResult value;
                    expr.getValue().accept(*this, value);
                    ((IntResult&)context).value = -value.value;
                }

                void LivenessWeightQueryVisitor::visit(const SubtractExpression& expr, Result& context){
                    IntResult left;
                    expr.getLeft().accept(*this, left);
                    IntResult right;
                    expr.getLeft().accept(*this, right);
                    ((IntResult&)context).value = left.value - right.value;
                }

                void LivenessWeightQueryVisitor::visit(const PlusExpression& expr, Result& context){
                    IntResult left;
                    expr.getLeft().accept(*this, left);
                    IntResult right;
                    expr.getLeft().accept(*this, right);
                    ((IntResult&)context).value = left.value + right.value;
                }

		void LivenessWeightQueryVisitor::visit(const BoolExpression& expr, Result& context)
		{
			static_cast<IntResult&>(context).value 
                                = expr.getValue()? 0 : std::numeric_limits<int>::max();
		}

		void LivenessWeightQueryVisitor::visit(const Query& query, Result& context)
		{
			query.getConstChild().accept(*this, context);
		}

                void LivenessWeightQueryVisitor::visit(const DeadlockExpression& expr, Result& context)
		{
                        static_cast<IntResult&>(context).value = 0;
		}
                
		int LivenessWeightQueryVisitor::compare(int numberOfTokensInPlace, const std::string& op, int n) const
		{
			if(op == "<") return numberOfTokensInPlace < n? abs(n-numberOfTokensInPlace):-abs(n-numberOfTokensInPlace);
			else if(op == "<=") return numberOfTokensInPlace <= n? abs(n-numberOfTokensInPlace):-abs(n-numberOfTokensInPlace);
			else if(op == "=" || op == "==") return numberOfTokensInPlace == n? 0:abs(n-numberOfTokensInPlace);
			else if(op == ">=") return numberOfTokensInPlace >= n? abs(n-numberOfTokensInPlace):-abs(n-numberOfTokensInPlace);
			else if(op == ">") return numberOfTokensInPlace > n? abs(n-numberOfTokensInPlace):-abs(n-numberOfTokensInPlace);
			else if(op == "!=") return numberOfTokensInPlace == n? abs(n-numberOfTokensInPlace):0;
			else
				throw std::exception();
		}

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
