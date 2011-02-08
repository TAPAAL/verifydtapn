#ifndef EXPRESSIONSATISFIEDVISITOR_HPP_
#define EXPRESSIONSATISFIEDVISITOR_HPP_

#include "Visitor.hpp"
#include <string>


namespace VerifyTAPN
{
	class SymbolicMarking;
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
			ExpressionSatisfiedVisitor(const SymbolicMarking& marking) : marking(marking) { };
			virtual ~ExpressionSatisfiedVisitor() {}
		public: // visitor methods
			virtual void Visit(const ParExpression& expr, boost::any& context);
			virtual void Visit(const OrExpression& expr, boost::any& context);
			virtual void Visit(const AndExpression& expr, boost::any& context);
			virtual void Visit(const AtomicProposition& expr, boost::any& context);
			virtual void Visit(const Query& query, boost::any& context);
		private:
			bool Compare(int numberOfTokensInPlace, const std::string& op, int n) const;

		private:
			const SymbolicMarking& marking;
		};
	}
}

#endif /* EXPRESSIONSATISFIEDVISITOR_HPP_ */
