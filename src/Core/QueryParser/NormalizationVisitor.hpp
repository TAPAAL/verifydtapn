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

		class NormalizationVisitor : public Visitor
		{
		public:
			NormalizationVisitor() : normalizedQuery() { };
			virtual ~NormalizationVisitor() {};

			virtual void visit(const NotExpression& expr, boost::any& context);
			virtual void visit(const ParExpression& expr, boost::any& context);
			virtual void visit(const OrExpression& expr, boost::any& context);
			virtual void visit(const AndExpression& expr, boost::any& context);
			virtual void visit(const AtomicProposition& expr, boost::any& context);
			virtual void visit(const BoolExpression& expr, boost::any& context);
			virtual void visit(const Query& query, boost::any& context);
			virtual void visit(const DeadlockExpression& expr, boost::any& context);

			AST::Query* normalize(const AST::Query& query) { boost::any any; query.accept(*this, any); 
                        return normalizedQuery; };
		private:
			std::string negateOperator(const std::string& op) const;
		private:
			AST::Query* normalizedQuery;
		};
	};
}


#endif /* NORMALIZATIONVISITOR_HPP_ */
