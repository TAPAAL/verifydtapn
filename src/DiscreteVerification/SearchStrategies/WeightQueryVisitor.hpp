/*
 * QueryVisitor.hpp
 *
 *  Created on: 21/03/2012
 *      Author: MathiasGS
 */

#ifndef WEIGHTQUERYVISITOR_HPP_
#define WEIGHTQUERYVISITOR_HPP_

#include "../../Core/QueryParser/Visitor.hpp"
#include "../DataStructures/NonStrictMarking.hpp"
#include "../../Core/QueryParser/AST.hpp"
#include <exception>
#include <limits.h>

namespace VerifyTAPN {
namespace DiscreteVerification {

using namespace AST;

class WeightQueryVisitor : public Visitor{
	public:
	WeightQueryVisitor(NonStrictMarkingBase& marking) : marking(marking){};
		virtual ~WeightQueryVisitor(){};

	public: // visitor methods
		virtual void visit(const NotExpression& expr, boost::any& context);
		virtual void visit(const ParExpression& expr, boost::any& context);
		virtual void visit(const OrExpression& expr, boost::any& context);
		virtual void visit(const AndExpression& expr, boost::any& context);
		virtual void visit(const AtomicProposition& expr, boost::any& context);
		virtual void visit(const BoolExpression& expr, boost::any& context);
		virtual void visit(const Query& query, boost::any& context);
                virtual void visit(const DeadlockExpression& expr, boost::any& context);
	private:
		int compare(int numberOfTokensInPlace, const std::string& op, int n) const;

	private:
		const NonStrictMarkingBase& marking;
};

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* QUERYVISITOR_HPP_ */
