/*
 * QueryVisitor.hpp
 *
 *  Created on: 21/03/2012
 *      Author: MathiasGS
 */

#ifndef QUERYVISITOR_HPP_
#define QUERYVISITOR_HPP_

#include "../Core/QueryParser/Visitor.hpp"
#include "NonStrictMarking.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

using namespace AST;

class QueryVisitor : public Visitor{
	public:
		QueryVisitor(NonStrictMarking& marking) : marking(marking){};
		virtual ~QueryVisitor(){};

	public: // visitor methods
		virtual void Visit(const NotExpression& expr, boost::any& context);
		virtual void Visit(const ParExpression& expr, boost::any& context);
		virtual void Visit(const OrExpression& expr, boost::any& context);
		virtual void Visit(const AndExpression& expr, boost::any& context);
		virtual void Visit(const AtomicProposition& expr, boost::any& context);
		virtual void Visit(const BoolExpression& expr, boost::any& context);
		virtual void Visit(const Query& query, boost::any& context);
	private:
		bool Compare(int numberOfTokensInPlace, const std::string& op, int n) const;

	private:
		const NonStrictMarking& marking;
};

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* QUERYVISITOR_HPP_ */
