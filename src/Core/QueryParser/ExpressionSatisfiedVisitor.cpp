#include "ExpressionSatisfiedVisitor.hpp"
#include "AST.hpp"
#include "../SymbolicMarking/SymMarking.hpp"
#include <exception>

namespace VerifyTAPN
{
	namespace AST
	{
		void ExpressionSatisfiedVisitor::Visit(const ParExpression& expr, boost::any& context)
		{
			expr.Child().Accept(*this, context);
		}

		void ExpressionSatisfiedVisitor::Visit(const OrExpression& expr, boost::any& context)
		{
			boost::any left, right;
			expr.Left().Accept(*this, left);
			expr.Right().Accept(*this, right);

			context = boost::any_cast<bool>(left) || boost::any_cast<bool>(right);
		}

		void ExpressionSatisfiedVisitor::Visit(const AndExpression& expr, boost::any& context)
		{
			boost::any left, right;
			expr.Left().Accept(*this, left);
			expr.Right().Accept(*this, right);

			context = boost::any_cast<bool>(left) && boost::any_cast<bool>(right);
		}

		void ExpressionSatisfiedVisitor::Visit(const AtomicProposition& expr, boost::any& context)
		{
			int numberOfTokens = marking.NumberOfTokensInPlace(expr.Place());
			context = Compare(numberOfTokens, expr.Operator(), expr.N());
		}

		void ExpressionSatisfiedVisitor::Visit(const Query& query, boost::any& context)
		{
			query.Child().Accept(*this, context);
		}

		bool ExpressionSatisfiedVisitor::Compare(int numberOfTokensInPlace, const std::string& op, int n) const
		{
			if(op == "<") return numberOfTokensInPlace < n;
			else if(op == "<=") return numberOfTokensInPlace <= n;
			else if(op == "=" || op == "==") return numberOfTokensInPlace == n;
			else if(op == ">=") return numberOfTokensInPlace >= n;
			else if(op == ">") return numberOfTokensInPlace > n;
			else
				throw std::exception();
		}
	}
}
