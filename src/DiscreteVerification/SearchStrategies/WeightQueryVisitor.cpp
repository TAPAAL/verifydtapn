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

		void WeightQueryVisitor::visit(const ParExpression& expr, Result& context)
		{
			expr.getChild().accept(*this, context);
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
			expr.getRight().accept(*this, right);


			if(left.value == std::numeric_limits<int>::max()){
				static_cast<IntResult&>(context).value = left.value;
				return;
			}

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
			int numberOfTokens = marking.numberOfTokensInPlace(expr.getPlace());
			static_cast<IntResult&>(context).value
                                = compare(numberOfTokens, expr.getOperator(), expr.getNumberOfTokens());
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
