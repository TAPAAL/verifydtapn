#ifndef VISITOR_HPP_
#define VISITOR_HPP_

#include "boost/any.hpp"

namespace VerifyTAPN
{
	namespace AST
	{
		class NotExpression;
		class ParExpression;
		class OrExpression;
		class AndExpression;
		class AtomicProposition;
		class BoolExpression;
		class Query;

		class Visitor
		{
		public:
			virtual ~Visitor() { };
			virtual void visit(const NotExpression& expr, boost::any& context) = 0;
			virtual void visit(const ParExpression& expr, boost::any& context) = 0;
			virtual void visit(const OrExpression& expr, boost::any& context) = 0;
			virtual void visit(const AndExpression& expr, boost::any& context) = 0;
			virtual void visit(const AtomicProposition& expr, boost::any& context) = 0;
			virtual void visit(const BoolExpression& expr, boost::any& context) = 0;
			virtual void visit(const Query& query, boost::any& context) = 0;
		};
	}
}


#endif /* VISITOR_HPP_ */
