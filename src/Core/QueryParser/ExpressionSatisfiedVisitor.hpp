#ifndef EXPRESSIONSATISFIEDVISITOR_HPP_
#define EXPRESSIONSATISFIEDVISITOR_HPP_

#include "Visitor.hpp"
#include <string>


namespace VerifyTAPN
{
	class SymMarking;
	namespace AST
	{
		class ParExpression;
		class OrExpression;
		class AndExpression;
		class AtomicProposition;
		class Query;

		class ExpressionSatisfiedVisitor : public Visitor
		{
		public:
			ExpressionSatisfiedVisitor(const SymMarking& marking) : marking(marking) { };
			virtual ~ExpressionSatisfiedVisitor() {}
		public: // visitor methods
			virtual void Visit(const ParExpression& expr, boost::any& context);
			virtual void Visit(const OrExpression& expr, boost::any& context);
			virtual void Visit(const AndExpression& expr, boost::any& context);
			virtual void Visit(const AtomicProposition& expr, boost::any& context);
			virtual void Visit(const Query& query, boost::any& context);

			bool IsSatisfied(const Query& query);
		private:
			bool Compare(int numberOfTokensInPlace, const std::string& op, int n) const;

		private:
			const SymMarking& marking;
		};
	}
}

#endif /* EXPRESSIONSATISFIEDVISITOR_HPP_ */
