/*
 * QueryVisitor.cpp
 *
 *  Created on: 21/03/2012
 *      Author: MathiasGS
 */

#include "WeightQueryVisitor.hpp"
#include "../Core/QueryParser/AST.hpp"
#include <exception>
#include <limits.h>

namespace VerifyTAPN {
namespace DiscreteVerification {

		void WeightQueryVisitor::Visit(const NotExpression& expr, boost::any& context)
		{
			assert(false);
		}

		void WeightQueryVisitor::Visit(const ParExpression& expr, boost::any& context)
		{
			expr.Child().Accept(*this, context);
		}

		void WeightQueryVisitor::Visit(const OrExpression& expr, boost::any& context)
		{
			boost::any left, right;
			expr.Left().Accept(*this, left);
			expr.Right().Accept(*this, right);

			context = min(boost::any_cast<int>(left), boost::any_cast<int>(right));
		}

		void WeightQueryVisitor::Visit(const AndExpression& expr, boost::any& context)
		{
			boost::any left, right;
			expr.Left().Accept(*this, left);
			expr.Right().Accept(*this, right);

			//TODO: Issue with int_max?
			context = boost::any_cast<int>(left) + boost::any_cast<int>(right);
		}

		void WeightQueryVisitor::Visit(const AtomicProposition& expr, boost::any& context)
		{
			int numberOfTokens = marking.NumberOfTokensInPlace(expr.Place());
			context = Compare(numberOfTokens, expr.Operator(), expr.N());
		}

		void WeightQueryVisitor::Visit(const BoolExpression& expr, boost::any& context)
		{
			context = expr.GetValue()? 0 : std::numeric_limits<int>::max();
		}

		void WeightQueryVisitor::Visit(const Query& query, boost::any& context)
		{
			query.Child().Accept(*this, context);
		}

		bool WeightQueryVisitor::Compare(int numberOfTokensInPlace, const std::string& op, int n) const
		{
			if(op == "<") return abs(n-numberOfTokensInPlace);
			else if(op == "<=") return abs(n-numberOfTokensInPlace);
			else if(op == "=" || op == "==") return abs(n-numberOfTokensInPlace);
			else if(op == ">=") return abs(n-numberOfTokensInPlace);
			else if(op == ">") return abs(n-numberOfTokensInPlace);
			else if(op == "!=") return abs(n-numberOfTokensInPlace);
			else
				throw std::exception();
		}

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
