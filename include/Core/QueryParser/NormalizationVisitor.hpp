/*
 * NormalizationVisitor.hpp
 *
 *  Created on: Nov 20, 2011
 *      Author: mortenja
 */

#ifndef NORMALIZATIONVISITOR_HPP_
#define NORMALIZATIONVISITOR_HPP_

#include "Visitor.hpp"
#include <string>
#include "AST.hpp"

namespace VerifyTAPN {
	namespace AST {
                class  Tuple : public Result {
                    public:
			bool negate;
			Expression* returnExpr;
			Tuple(bool negate, Expression* expr) : negate(negate), returnExpr(expr) {};
		};
                
		class NormalizationVisitor : public Visitor
		{
		public:
			NormalizationVisitor() : normalizedQuery() { };
			virtual ~NormalizationVisitor() {};

			virtual void visit(NotExpression& expr, Result& context);
			virtual void visit(OrExpression& expr, Result& context);
			virtual void visit(AndExpression& expr, Result& context);
			virtual void visit(AtomicProposition& expr, Result& context);
			virtual void visit(BoolExpression& expr, Result& context);
			virtual void visit(Query& query, Result& context);
			virtual void visit(DeadlockExpression& expr, Result& context);
                        virtual void visit(NumberExpression& expr, Result& context);
                        virtual void visit(IdentifierExpression& expr, Result& context);
                        virtual void visit(MultiplyExpression& expr, Result& context);
                        virtual void visit(MinusExpression& expr, Result& context);
                        virtual void visit(SubtractExpression& expr, Result& context);
                        virtual void visit(PlusExpression& expr, Result& context);

			AST::Query* normalize(AST::Query& query) { Tuple any(false, NULL); query.accept(*this, any); 
                        return normalizedQuery; };
		private:
			std::string negateOperator(const std::string& op) const;
		private:
			AST::Query* normalizedQuery;
		};
	};
}


#endif /* NORMALIZATIONVISITOR_HPP_ */
