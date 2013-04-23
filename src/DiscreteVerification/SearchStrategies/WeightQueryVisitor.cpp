/*
 * QueryVisitor.cpp
 *
 *  Created on: 21/03/2012
 *      Author: MathiasGS
 */

#include "WeightQueryVisitor.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

		void WeightQueryVisitor::visit(const NotExpression& expr, boost::any& context)
		{
			assert(false);
		}

		void WeightQueryVisitor::visit(const ParExpression& expr, boost::any& context)
		{
			expr.Child().Accept(*this, context);
		}

		void WeightQueryVisitor::visit(const OrExpression& expr, boost::any& context)
		{
			boost::any left, right;
			expr.Left().Accept(*this, left);
			expr.Right().Accept(*this, right);

			context = min(boost::any_cast<int>(left), boost::any_cast<int>(right));
		}

		void WeightQueryVisitor::visit(const AndExpression& expr, boost::any& context)
		{
			boost::any left, right;
			expr.Left().Accept(*this, left);
			expr.Right().Accept(*this, right);

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

		void WeightQueryVisitor::visit(const AtomicProposition& expr, boost::any& context)
		{
			int numberOfTokens = marking.numberOfTokensInPlace(expr.Place());
			context = compare(numberOfTokens, expr.Operator(), expr.N());
		}

		void WeightQueryVisitor::visit(const BoolExpression& expr, boost::any& context)
		{
			context = expr.GetValue()? 0 : std::numeric_limits<int>::max();
		}

		void WeightQueryVisitor::visit(const Query& query, boost::any& context)
		{
			query.Child().Accept(*this, context);
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
