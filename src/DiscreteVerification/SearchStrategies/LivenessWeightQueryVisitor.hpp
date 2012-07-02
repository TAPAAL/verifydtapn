/*
 * QueryVisitor.hpp
 *
 *  Created on: 21/03/2012
 *      Author: MathiasGS
 */

#ifndef LIVEWEIGHTQUERYVISITOR_HPP_
#define LIVEWEIGHTQUERYVISITOR_HPP_

#include "../../Core/QueryParser/Visitor.hpp"
#include "../DataStructures/NonStrictMarking.hpp"
#include "../../Core/QueryParser/AST.hpp"
#include <exception>
#include <limits.h>

namespace VerifyTAPN {
namespace DiscreteVerification {

using namespace AST;

class LivenessWeightQueryVisitor : public Visitor{
	public:
	LivenessWeightQueryVisitor(NonStrictMarking& marking) : marking(marking){};
		virtual ~LivenessWeightQueryVisitor(){};

	public: // visitor methods
		virtual void Visit(const NotExpression& expr, boost::any& context);
		virtual void Visit(const ParExpression& expr, boost::any& context);
		virtual void Visit(const OrExpression& expr, boost::any& context);
		virtual void Visit(const AndExpression& expr, boost::any& context);
		virtual void Visit(const AtomicProposition& expr, boost::any& context);
		virtual void Visit(const BoolExpression& expr, boost::any& context);
		virtual void Visit(const Query& query, boost::any& context);
	private:
		int Compare(int numberOfTokensInPlace, const std::string& op, int n) const;

	private:
		const NonStrictMarking& marking;
};

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* LIVENESSQUERYVISITOR_HPP_ */
