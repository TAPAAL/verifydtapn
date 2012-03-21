/*
 * QueryVisitor.cpp
 *
 *  Created on: 21/03/2012
 *      Author: MathiasGS
 */

#include "QueryVisitor.hpp"
#include "../Core/QueryParser/AST.hpp"
#include <exception>

namespace VerifyTAPN {
namespace DiscreteVerification {

		void QueryVisitor::Visit(const NotExpression& expr, boost::any& context)
		{
			boost::any c;
			expr.Child().Accept(*this, c);
			context = !boost::any_cast<bool>(c);
		}

		void QueryVisitor::Visit(const ParExpression& expr, boost::any& context)
		{
			expr.Child().Accept(*this, context);
		}

		void QueryVisitor::Visit(const OrExpression& expr, boost::any& context)
		{
			boost::any left, right;
			expr.Left().Accept(*this, left);
			expr.Right().Accept(*this, right);

			context = boost::any_cast<bool>(left) || boost::any_cast<bool>(right);
		}

		void QueryVisitor::Visit(const AndExpression& expr, boost::any& context)
		{
			boost::any left, right;
			expr.Left().Accept(*this, left);
			expr.Right().Accept(*this, right);

			context = boost::any_cast<bool>(left) && boost::any_cast<bool>(right);
		}

		void QueryVisitor::Visit(const AtomicProposition& expr, boost::any& context)
		{
			int numberOfTokens = marking.NumberOfTokensInPlace(expr.Place());
			context = Compare(numberOfTokens, expr.Operator(), expr.N());
		}

		void QueryVisitor::Visit(const BoolExpression& expr, boost::any& context)
		{
			context = expr.GetValue();
		}

		void QueryVisitor::Visit(const Query& query, boost::any& context)
		{
			query.Child().Accept(*this, context);
		}

		bool QueryVisitor::Compare(int numberOfTokensInPlace, const std::string& op, int n) const
		{
			if(op == "<") return numberOfTokensInPlace < n;
			else if(op == "<=") return numberOfTokensInPlace <= n;
			else if(op == "=" || op == "==") return numberOfTokensInPlace == n;
			else if(op == ">=") return numberOfTokensInPlace >= n;
			else if(op == ">") return numberOfTokensInPlace > n;
			else if(op == "!=") return numberOfTokensInPlace != n;
			else
				throw std::exception();
		}

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
