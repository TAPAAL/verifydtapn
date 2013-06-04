/*
 * QueryVisitor.cpp
 *
 *  Created on: 21/03/2012
 *      Author: MathiasGS
 */

#include "LivenessWeightQueryVisitor.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

		void LivenessWeightQueryVisitor::visit(const NotExpression& expr, boost::any& context)
		{
			assert(false);
		}

		void LivenessWeightQueryVisitor::visit(const ParExpression& expr, boost::any& context)
		{
			expr.getChild().accept(*this, context);
		}

		void LivenessWeightQueryVisitor::visit(const OrExpression& expr, boost::any& context)
		{
			boost::any left, right;
			expr.getLeft().accept(*this, left);
			expr.getRight().accept(*this, right);

			context = min(boost::any_cast<int>(left), boost::any_cast<int>(right));
		}

		void LivenessWeightQueryVisitor::visit(const AndExpression& expr, boost::any& context)
		{
			boost::any left, right;
			expr.getLeft().accept(*this, left);
			expr.getRight().accept(*this, right);

			int l = boost::any_cast<int>(left);
			if(l == std::numeric_limits<int>::max()){
				context = l;
				return;
			}

			int r = boost::any_cast<int>(right);
			if(r == std::numeric_limits<int>::max()){
				context = r;
				return;
			}

			context = l+r;
		}


		void LivenessWeightQueryVisitor::visit(const AtomicProposition& expr, boost::any& context)
		{
			int numberOfTokens = marking.numberOfTokensInPlace(expr.getPlace());
			context = compare(numberOfTokens, expr.getOperator(), expr.getNumberOfTokens());
		}

		void LivenessWeightQueryVisitor::visit(const BoolExpression& expr, boost::any& context)
		{
			context = expr.getValue()? 0 : std::numeric_limits<int>::max();
		}

		void LivenessWeightQueryVisitor::visit(const Query& query, boost::any& context)
		{
			query.getChild().accept(*this, context);
		}

                void LivenessWeightQueryVisitor::visit(const DeadlockExpression& expr, boost::any& context)
		{
			// no weight
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
