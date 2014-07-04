/*
 * QueryVisitor.cpp
 *
 *  Created on: 21/03/2012
 *      Author: MathiasGS
 */

#include "WeightQueryVisitor.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

		void WeightQueryVisitor::visit(const NotExpression& expr, Result& context)
		{
			assert(false);
		}

		void WeightQueryVisitor::visit(const OrExpression& expr, Result& context)
		{
			IntResult left, right;
			expr.getLeft().accept(*this, left);
			expr.getRight().accept(*this, right);

			static_cast<IntResult&>(context).value
                                = min(left.value, right.value);
		}

		void WeightQueryVisitor::visit(const AndExpression& expr, Result& context)
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

			static_cast<IntResult&>(context).value 
                                = left.value+right.value;
		}

                void WeightQueryVisitor::visit(const DeadlockExpression& expr, Result& context)
		{
                    static_cast<IntResult&>(context).value = 0;
		}

		void WeightQueryVisitor::visit(const AtomicProposition& expr, Result& context)
		{
                    IntResult left;
                    expr.getLeft().accept(*this, left);
                    IntResult right;
                    expr.getRight().accept(*this, right);
			static_cast<IntResult&>(context).value
                                = compare(left.value, expr.getOperator(), right.value);
		}

                void WeightQueryVisitor::visit(const NumberExpression& expr, Result& context){
                    ((IntResult&)context).value = expr.getValue();
                }

                void WeightQueryVisitor::visit(const IdentifierExpression& expr, Result& context){
                    ((IntResult&)context).value = marking.numberOfTokensInPlace(expr.getPlace());
                }
                
                void WeightQueryVisitor::visit(const MultiplyExpression& expr, Result& context){
                    IntResult left;
                    expr.getLeft().accept(*this, left);
                    IntResult right;
                    expr.getRight().accept(*this, right);
                    ((IntResult&)context).value = left.value * right.value;
                }
                
                void WeightQueryVisitor::visit(const MinusExpression& expr, Result& context){
                    IntResult value;
                    expr.getValue().accept(*this, value);
                    ((IntResult&)context).value = -value.value;
                }

                void WeightQueryVisitor::visit(const SubtractExpression& expr, Result& context){
                    IntResult left;
                    expr.getLeft().accept(*this, left);
                    IntResult right;
                    expr.getRight().accept(*this, right);
                    ((IntResult&)context).value = left.value - right.value;
                }

                void WeightQueryVisitor::visit(const PlusExpression& expr, Result& context){
                    IntResult left;
                    expr.getLeft().accept(*this, left);
                    IntResult right;
                    expr.getRight().accept(*this, right);
                    ((IntResult&)context).value = left.value + right.value;
                }
                
		void WeightQueryVisitor::visit(const BoolExpression& expr, Result& context)
		{
			static_cast<IntResult&>(context).value
                                = expr.getValue()? 0 : std::numeric_limits<int>::max();
		}

		void WeightQueryVisitor::visit(const Query& query, Result& context)
		{
			query.getChild().accept(*this, context);
		}

		int WeightQueryVisitor::compare(int numberOfTokensInPlace, const std::string& op, int n) const
		{
			if(op == "<") return numberOfTokensInPlace < n? 0:abs(n-numberOfTokensInPlace);
			else if(op == "<=") return numberOfTokensInPlace <= n? 0:abs(n-numberOfTokensInPlace);
			else if(op == "=" || op == "==") return numberOfTokensInPlace == n? 0:abs(n-numberOfTokensInPlace);
			else if(op == ">=") return numberOfTokensInPlace >= n? 0:abs(n-numberOfTokensInPlace);
			else if(op == ">") return numberOfTokensInPlace > n? 0:abs(n-numberOfTokensInPlace);
			else if(op == "!=") return numberOfTokensInPlace == n? 1:0;
			else
				throw std::exception();
		}

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
