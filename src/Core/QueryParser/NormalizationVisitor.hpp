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

			virtual void visit(const NotExpression& expr, Result& context);
			virtual void visit(const OrExpression& expr, Result& context);
			virtual void visit(const AndExpression& expr, Result& context);
			virtual void visit(const AtomicProposition& expr, Result& context);
			virtual void visit(const BoolExpression& expr, Result& context);
			virtual void visit(const Query& query, Result& context);
			virtual void visit(const DeadlockExpression& expr, Result& context);
                        virtual void visit(const NumberExpression& expr, Result& context);
                        virtual void visit(const IdentifierExpression& expr, Result& context);
                        virtual void visit(const MultiplyExpression& expr, Result& context);
                        virtual void visit(const MinusExpression& expr, Result& context);
                        virtual void visit(const SubtractExpression& expr, Result& context);
                        virtual void visit(const PlusExpression& expr, Result& context);

			AST::Query* normalize(const AST::Query& query) { Tuple any(false, NULL); query.accept(*this, any); 
                        return normalizedQuery; };
		private:
			std::string negateOperator(const std::string& op) const;
		private:
			AST::Query* normalizedQuery;
		};
	};
}


#endif /* NORMALIZATIONVISITOR_HPP_ */
