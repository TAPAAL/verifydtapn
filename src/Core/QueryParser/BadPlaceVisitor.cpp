#include "BadPlaceVisitor.hpp"

namespace VerifyTAPN{
	namespace AST {

		void BadPlaceVisitor::Visit(const NotExpression& expr, boost::any& context)
		{
			expr.Child().Accept(*this, context);
		}

		void BadPlaceVisitor::Visit(const ParExpression& expr, boost::any& context)
		{
			expr.Child().Accept(*this, context);
		}

		void BadPlaceVisitor::Visit(const OrExpression& expr, boost::any& context)
		{
			expr.Left().Accept(*this, context);
			expr.Right().Accept(*this, context);
		}

		void BadPlaceVisitor::Visit(const AndExpression& expr, boost::any& context)
		{
			expr.Left().Accept(*this, context);
			expr.Right().Accept(*this, context);
		}

		void BadPlaceVisitor::Visit(const AtomicProposition& expr, boost::any& context)
		{
			if(expr.Operator() == "=" || expr.Operator() == "==" || expr.Operator() == "!=" || expr.Operator() == "<" || expr.Operator() == "<="){
				badPlaces.push_back(expr.Place());
			}
		}

		void BadPlaceVisitor::Visit(const BoolExpression& expr, boost::any& context)
		{
		}

		void BadPlaceVisitor::Visit(const Query& query, boost::any& context)
		{
			query.Child().Accept(*this, context);
		}
	}
}
