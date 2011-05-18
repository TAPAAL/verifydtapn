#ifndef TOSTRINGVISITOR_HPP_
#define TOSTRINGVISITOR_HPP_

#include "Visitor.hpp"

namespace VerifyTAPN
{
	namespace AST
	{

		class ToStringVisitor : public Visitor
		{
		public:
			ToStringVisitor() { };
			virtual ~ToStringVisitor() {}
			virtual void Visit(const NotExpression& expr, boost::any& context);
			virtual void Visit(const ParExpression& expr, boost::any& context);
			virtual void Visit(const OrExpression& expr, boost::any& context);
			virtual void Visit(const AndExpression& expr, boost::any& context);
			virtual void Visit(const AtomicProposition& expr, boost::any& context);
			virtual void Visit(const BoolExpression& expr, boost::any& context);
			virtual void Visit(const Query& query, boost::any& context);

		};
	}
}
#endif /* TOSTRINGVISITOR_HPP_ */
