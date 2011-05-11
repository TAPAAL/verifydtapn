#ifndef VISITOR_HPP_
#define VISITOR_HPP_

#include "boost/any.hpp"

namespace VerifyTAPN
{
	namespace AST
	{
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
			virtual void Visit(const ParExpression& expr, boost::any& context) = 0;
			virtual void Visit(const OrExpression& expr, boost::any& context) = 0;
			virtual void Visit(const AndExpression& expr, boost::any& context) = 0;
			virtual void Visit(const AtomicProposition& expr, boost::any& context) = 0;
			virtual void Visit(const BoolExpression& expr, boost::any& context) = 0;
			virtual void Visit(const Query& query, boost::any& context) = 0;
		};
	}
}


#endif /* VISITOR_HPP_ */
