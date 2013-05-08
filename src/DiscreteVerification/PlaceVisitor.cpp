/*
 * PlaceVisitor.cpp
 *
 *  Created on: 21/03/2012
 *      Author: MathiasGS
 */

#include "PlaceVisitor.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

		void PlaceVisitor::visit(const NotExpression& expr, boost::any& context)
		{
			expr.getChild().accept(*this, context);
		}

		void PlaceVisitor::visit(const ParExpression& expr, boost::any& context)
		{
			expr.getChild().accept(*this, context);
		}

		void PlaceVisitor::visit(const OrExpression& expr, boost::any& context)
		{
			expr.getLeft().accept(*this, context);
			expr.getRight().accept(*this, context);
		}

		void PlaceVisitor::visit(const AndExpression& expr, boost::any& context)
		{
			expr.getLeft().accept(*this, context);
			expr.getRight().accept(*this, context);
		}

		void PlaceVisitor::visit(const AtomicProposition& expr, boost::any& context)
		{
			std::vector<int> v = boost::any_cast< std::vector< int > >(context);
			v.push_back(expr.getPlace());
			context = v;
		}

		void PlaceVisitor::visit(const BoolExpression& expr, boost::any& context)
		{
		}

		void PlaceVisitor::visit(const Query& query, boost::any& context)
		{
			query.getChild().accept(*this, context);
		}
} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
