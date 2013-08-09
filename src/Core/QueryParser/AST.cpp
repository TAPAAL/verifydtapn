#include "AST.hpp"
namespace VerifyTAPN
{
	namespace AST
	{
		NotExpression* NotExpression::clone() const
		{
			return new NotExpression(*this);
		}

		void NotExpression::accept(Visitor& visitor, Result& context) const
		{
			visitor.visit(*this, context);
		}

		void BoolExpression::accept(Visitor& visitor, Result& context) const
		{
			visitor.visit(*this, context);
		}

		BoolExpression* BoolExpression::clone() const
		{
			return new BoolExpression(*this);
		}

                void DeadlockExpression::accept(Visitor& visitor, Result& context) const
		{      
			visitor.visit(*this, context);
		}

		DeadlockExpression* DeadlockExpression::clone() const
		{
			return new DeadlockExpression(*this);
		}
        
		void AtomicProposition::accept(Visitor& visitor, Result& context) const
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

		void AndExpression::accept(Visitor& visitor, Result& context) const
		{
			visitor.visit(*this, context);
		}

		OrExpression* OrExpression::clone() const
		{
			return new OrExpression(*this);
		}

		void OrExpression::accept(Visitor& visitor, Result& context) const
		{
			visitor.visit(*this, context);
		}

		ParExpression* ParExpression::clone() const
		{
			return new ParExpression(*this);
		}

		void ParExpression::accept(Visitor& visitor, Result& context) const
		{
			visitor.visit(*this, context);
		}

		Query* Query::clone() const
		{
			return new Query(*this);
		}

		void Query::accept(Visitor& visitor, Result& context) const
		{
			visitor.visit(*this, context);
		}
	}
}
