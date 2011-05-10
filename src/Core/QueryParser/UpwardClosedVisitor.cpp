#include "UpwardClosedVisitor.hpp"
#include "AST.hpp"

namespace VerifyTAPN {
namespace AST {
		void UpwardClosedVisitor::Visit(const ParExpression& expr, boost::any& context)
		{
			expr.Child().Accept(*this, context);
		}

		void UpwardClosedVisitor::Visit(const OrExpression& expr, boost::any& context)
		{
			boost::any left, right;
			expr.Left().Accept(*this, left);
			expr.Right().Accept(*this, right);

			context = boost::any_cast<bool>(left) && boost::any_cast<bool>(right); // this should be &&; We are checking that each child is upward closed
		}

		void UpwardClosedVisitor::Visit(const AndExpression& expr, boost::any& context)
		{
			boost::any left, right;
			expr.Left().Accept(*this, left);
			expr.Right().Accept(*this, right);

			context = boost::any_cast<bool>(left) && boost::any_cast<bool>(right);
		}

	void UpwardClosedVisitor::Visit(const AtomicProposition& expr, boost::any& context)
	{
		if(propShouldBeUpward){
			context = expr.Operator() == ">=" || expr.Operator() == ">";
		}else{
			context = expr.Operator() == "<=" || expr.Operator() == "<";
		}
	};

	void UpwardClosedVisitor::Visit(const Query& query, boost::any& context)
	{
		if(query.GetQuantifier() == EF)
			propShouldBeUpward = true;
		else
			propShouldBeUpward = false;

		query.Child().Accept(*this, context);
	};

	bool UpwardClosedVisitor::IsUpwardClosed(const Query& query)
	{
		boost::any any;
		Visit(query, any);
		return boost::any_cast<bool>(any);
	};
}
}
