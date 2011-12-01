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
			virtual void Visit(const NotExpression& expr, boost::any& context);
			virtual void Visit(const ParExpression& expr, boost::any& context);
			virtual void Visit(const OrExpression& expr, boost::any& context);
			virtual void Visit(const AndExpression& expr, boost::any& context);
			virtual void Visit(const AtomicProposition& expr, boost::any& context);
			virtual void Visit(const BoolExpression& expr, boost::any& context);
			virtual void Visit(const Query& query, boost::any& context);

			AST::Query* Normalize(const AST::Query& query) { boost::any any; query.Accept(*this, any); return normalizedQuery; };
		private:
			std::string NegateOperator(const std::string& op) const;
		private:
			AST::Query* normalizedQuery;
		};
	};
}


#endif /* NORMALIZATIONVISITOR_HPP_ */
