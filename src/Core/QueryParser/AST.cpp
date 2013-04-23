#include "AST.hpp"
namespace VerifyTAPN
{
	namespace AST
	{
		NotExpression* NotExpression::clone() const
		{
			return new NotExpression(*this);
		}

		void NotExpression::Accept(Visitor& visitor, boost::any& context) const
		{
			visitor.visit(*this, context);
		}

		void BoolExpression::Accept(Visitor& visitor, boost::any& context) const
		{
			visitor.visit(*this, context);
		}

		BoolExpression* BoolExpression::clone() const
		{
			return new BoolExpression(*this);
		}

		void AtomicProposition::Accept(Visitor& visitor, boost::any& context) const
		{
			visitor.visit(*this, context);
		}

		AtomicProposition* AtomicProposition::clone() const
		{
			return new AtomicProposition(*this);
		}

		AndExpression* AndExpression::clone() const
		{
			return new AndExpression(*this);
		}

		void AndExpression::Accept(Visitor& visitor, boost::any& context) const
		{
			visitor.visit(*this, context);
		}

		OrExpression* OrExpression::clone() const
		{
			return new OrExpression(*this);
		}

		void OrExpression::Accept(Visitor& visitor, boost::any& context) const
		{
			visitor.visit(*this, context);
		}

		ParExpression* ParExpression::clone() const
		{
			return new ParExpression(*this);
		}

		void ParExpression::Accept(Visitor& visitor, boost::any& context) const
		{
			visitor.visit(*this, context);
		}

		Query* Query::clone() const
		{
			return new Query(*this);
		}

		void Query::Accept(Visitor& visitor, boost::any& context) const
		{
			visitor.visit(*this, context);
		}
	}
}
